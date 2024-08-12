Config.helpLinks.push({
    url: "mailto:techsupport@alphavs.com",
    title: "Video Centrix Support",
    target: "new",
    button: "techsupport@alphavs.com"
});

Config.helpLinks.push({
    urls: [
        {
            url: "https://itunes.apple.com/eg/app/1608609205?mt=8",
            button: "iOS Client",
            class:'appstore'
        },
        {
			url: "https://play.google.com/store/apps/details?id=com.alphavs.videocentrix",
			title: "Android Client",
			target: "new" // new|frame
        }
    ],
    title: "Mobile Apps",
    target: "new" // new|frame
});

Config.allowDebugMode = false;
//Config.webclientEnabled = false; // Uncomment this string to disable web client
Config.productName = 'Video Centrix';
Config.cloud.productName = 'Alpha VS Cloud';
Config.defaultLanguage = 'en_US';
Config.supportedLanguages = ['en_US', 'cs_CZ', 'de_DE', 'en_GB', 'es_ES', 'fi_FI', 'fr_FR', 'he_IL', 'hu_HU', 'it_IT', 'ja_JP', 'ko_KR', 'nl_BE', 'nl_NL', 'no_NO', 'pl_PL', 'pt_BR', 'pt_PT', 'ru_RU', 'sv_SE', 'th_TH', 'tr_TR', 'uk_UA', 'vi_VN', 'zh_CN', 'zh_TW'];
Config.developersFeedbackForm = false;