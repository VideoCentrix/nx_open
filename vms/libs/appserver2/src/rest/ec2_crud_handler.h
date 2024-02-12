// Copyright 2018-present Network Optix, Inc. Licensed under MPL 2.0: www.mozilla.org/MPL/2.0/

#pragma once

#include <tuple>

#include <core/resource/resource_type.h>
#include <database/db_manager.h>
#include <nx/network/rest/params.h>
#include <nx/network/rest/request.h>
#include <nx/utils/elapsed_timer.h>
#include <nx/utils/scope_guard.h>
#include <nx/vms/ec2/ec_connection_notification_manager.h>
#include <transaction/transaction.h>

#include "details.h"
#include "subscription.h"

namespace ec2 {

template<
    typename Filter,
    typename Model,
    typename DeleteInput,
    typename QueryProcessor,
    ApiCommand::Value DeleteCommand
>
class CrudHandler: public SubscriptionHandler
{
public:
    CrudHandler(QueryProcessor* queryProcessor):
        SubscriptionHandler(
            [this]()
            {
                auto commands = details::commands<Model>(DeleteCommand);
                NX_VERBOSE(this, "Add monitor for %1", commands);
                auto guard = m_queryProcessor->addMonitor(
                    [this](const auto& transaction) { notify(transaction); }, std::move(commands));
                return nx::utils::Guard{
                    [this, guard = std::move(guard)]() { NX_VERBOSE(this, "Remove monitor"); }};
            }),
        m_queryProcessor(queryProcessor)
    {
    }

    std::vector<Model> read(Filter filter, const nx::network::rest::Request& request)
    {
        using namespace details;

        auto processor = m_queryProcessor->getAccess(request.userSession);
        validateType(processor, filter, m_objectType);

        // `std::type_identity` (or similar) can be used to pass the read type info and avoid
        // useless `readType` object instantiation.
        const auto query =
            [id = filter.getId(), &processor](const auto& readType)
            {
                using Arg = std::decay_t<decltype(readType)>;
                using Output = std::conditional_t<nx::utils::IsVector<Arg>::value, Arg, std::vector<Arg>>;
                return processor.template processQueryAsync<decltype(id), Output>(
                    getReadCommand<Output>(),
                    std::move(id),
                    std::make_pair<Result, Output>);
            };

        const auto readFuture =
            [](auto future)
            {
                future.waitForFinished();
                auto [result, output] = future.resultAt(0);
                if (!result)
                    throwError(std::move(result));
                return output;
            };

        if constexpr (fromDbTypesExists<Model>::value)
        {
            const auto logGuard = logTime("Query and Convert from DB types");

            auto futures = std::apply(
                [&](const auto&... readTypes)
                {
                    return std::make_tuple(query(readTypes)...);
                },
                typename Model::DbReadTypes());
            return Model::fromDbTypes(
                std::apply(
                    [readFuture](auto&&... futures) -> typename Model::DbListTypes
                    {
                        return {readFuture(std::forward<decltype(futures)>(futures))...};
                    },
                    std::move(futures)));
        }
        else
        {
            return readFuture(query(Model()));
        }
    }

    void delete_(DeleteInput id, const nx::network::rest::Request& request)
    {
        using namespace details;

        auto processor = m_queryProcessor->getAccess(request.userSession);
        validateType(processor, id, m_objectType);

        std::promise<Result> promise;
        processor.processUpdateAsync(
            DeleteCommand,
            std::move(id),
            [&promise](Result result) { promise.set_value(std::move(result)); });
        if (Result result = promise.get_future().get(); !result)
            throwError(std::move(result));
    }

    void create(Model data, const nx::network::rest::Request& request)
    {
        update(std::move(data), request);
    }

    void update(Model data, const nx::network::rest::Request& request)
    {
        using namespace details;

        std::promise<Result> promise;
        auto processor = m_queryProcessor->getAccess(request.userSession);
        if constexpr (toDbTypesExists<Model>::value)
        {
            auto items = std::move(data).toDbTypes();
            const auto& mainDbItem = std::get<0>(items);
            validateType(processor, mainDbItem, m_objectType);
            validateResourceTypeId(mainDbItem);

            using IgnoredDbType = std::decay_t<decltype(mainDbItem)>;
            const auto update =
                [id = data.getId()](auto&& x, auto& copy, auto* list)
                {
                    using DbType = std::decay_t<decltype(x)>;
                    Result result;
                    assertModelToDbTypesProducedValidResult<IgnoredDbType, DbType, Model>(x, id);
                    if constexpr (nx::utils::IsVector<DbType>::value)
                    {
                        const auto command = getUpdateCommand<std::decay_t<decltype(x[0])>>();
                        result = copy.template processMultiUpdateSync<DbType>(
                            parentUpdateCommand(command),
                            command,
                            TransactionType::Regular,
                            std::move(x),
                            list);
                    }
                    else
                    {
                        result =
                            copy.processUpdateSync(getUpdateCommand<DbType>(), std::move(x), list);
                    }
                    return result;
                };
            processor.processCustomUpdateAsync(
                ApiCommand::CompositeSave,
                [&promise](Result result) { promise.set_value(std::move(result)); },
                [update, items = std::move(items)](auto& copy, auto* list)
                {
                    const auto updateItem =
                        [update, &copy, list](auto&& item) -> Result
                        {
                            if constexpr (nx::utils::IsOptional<std::decay_t<decltype(item)>>::value)
                                return item ? update(std::move(*item), copy, list) : Result();
                            else
                                return update(std::move(item), copy, list);
                        };

                    return std::apply(
                        [updateItem](auto&&... items) -> Result
                        {
                            Result result{};
                            (... && (result = updateItem(std::forward<decltype(items)>(items))));
                            return result;
                        },
                        std::move(items));
                });
        }
        else
        {
            validateType(processor, data, m_objectType);
            validateResourceTypeId(data);
            processor.processUpdateAsync(
                getUpdateCommand<Model>(),
                data,
                [&promise](Result result) { promise.set_value(std::move(result)); });
        }
        if (Result result = promise.get_future().get(); !result)
            throwError(std::move(result));
    }

    QString getSubscriptionId(const nx::network::rest::Request& request)
    {
        if (request.method() == nx::network::http::Method::get)
        {
            return request.params().toJson(/*excludeCommon*/ true).isEmpty()
                ? QString("*")
                : nx::toString(request.parseContentOrThrow<Filter>().getId());
        }

        if (request.method() == nx::network::http::Method::delete_)
        {
            return request.params().toJson(/*excludeCommon*/ true).isEmpty()
                ? QString("*")
                : nx::toString(request.parseContentOrThrow<DeleteInput>().getId());
        }

        NX_ASSERT(false, "Use `get` to `subscribe` or `delete` to `unsubscribe`");
        return {};
    }

protected:
    QueryProcessor* const m_queryProcessor;

private:
    bool isValidType(const nx::Uuid& id) const
    {
        const auto objectType = m_queryProcessor->getAccess(Qn::kSystemAccess).getObjectType(id);
        return objectType == ApiObject_NotDefined || objectType == m_objectType;
    }

    bool isValidType(const nx::vms::api::LicenseKey&) const
    {
        return DeleteCommand == ApiCommand::removeLicense;
    }

    void notify(const QnAbstractTransaction& transaction)
    {
        using namespace details;

        if (DeleteCommand == transaction.command)
        {
            const auto id =
                static_cast<const QnTransaction<DeleteInput>&>(transaction).params.getId();
            if (isValidType(id))
            {
                NX_VERBOSE(this, "Notify %1 for %2", id, transaction.command);
                SubscriptionHandler::notify(nx::toString(id), NotifyType::delete_, {});
            }
            return;
        }

        invokeOnDbTypes<applyOr, Model>(
            [this, &transaction](auto&& data)
            {
                using T = std::decay_t<decltype(data)>;
                if (getUpdateCommand<T>() != transaction.command)
                    return false;

                const auto id = static_cast<const QnTransaction<T>&>(transaction).params.getId();
                if (isValidType(id))
                {
                    NX_VERBOSE(this, "Notify %1 for %2", id, transaction.command);
                    SubscriptionHandler::notify(nx::toString(id), NotifyType::update, {});
                }
                return true;
            });
    }

    nx::utils::Guard logTime(const char* label) const
    {
        return nx::utils::Guard(
            [this, label, timer = nx::utils::ElapsedTimer(nx::utils::ElapsedTimerState::started)]
            {
                NX_VERBOSE(this, "`%1` in `%2`", label, timer.elapsed());
            });
    }

private:
    static constexpr ApiObjectType m_objectType = details::commandToObjectType(DeleteCommand);
};

} // namespace ec2
