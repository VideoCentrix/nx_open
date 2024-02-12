// Copyright 2018-present Network Optix, Inc. Licensed under MPL 2.0: www.mozilla.org/MPL/2.0/

#pragma once

#include <ui/graphics/items/controls/html_text_item.h>
#include <ui/graphics/items/overlays/scrollable_items_widget.h>

/**
  * Container class that creates and displays QnHtmlTextItem items on a scrollable area.
  */
class QnScrollableTextItemsWidget: public QnScrollableItemsWidget
{
    using base_type = QnScrollableItemsWidget;

public:
    QnScrollableTextItemsWidget(QGraphicsWidget* parent = nullptr);
    virtual ~QnScrollableTextItemsWidget();

    /** Add text item to scrollable area at the end.
      * If data.id is not null it's used as unique item id, otherwise it is autogenerated.
      * If item is successfully added, its unique id is returned.
      * If another item with specified externalId already exists,
      *    new item isn't added and null uuid is returned. */
    nx::Uuid addItem(const QString& text, const QnHtmlTextItemOptions& options,
        const nx::Uuid& externalId = nx::Uuid());

    /** Insert item to scrollable area at specified position.
      * If data.id is not null it's used as unique item id, otherwise it is autogenerated.
      * If item is successfully added, its unique id is returned.
      * If another item with specified externalId already exists,
      *    new item isn't inserted and null uuid is returned. */
    nx::Uuid insertItem(int index, const QString& text, const QnHtmlTextItemOptions& options,
        const nx::Uuid& externalId = nx::Uuid());
};
