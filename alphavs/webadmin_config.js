{
    "dynamicMenus": [
        {
            "name": "header",
            "nodes": [
                {
                    "name": "Services",
                    "display_name": "Services",
                    "url": "",
                    "new_window": false,
                    "icon": "services.svg",
                    "authentication": "both",
                    "order": 1,
                    "enabled": true,
                    "nodes": [
                        {
                            "name": "Downloads",
                            "display_name": "Downloads",
                            "url": "{{CLOUD_HOST}}/download",
                            "new_window": true,
                            "icon": "",
                            "authentication": "both",
                            "order": 0,
                            "enabled": true
                        },
                        {
                            "name": "Supported Devices",
                            "display_name": "Supported Devices",
                            "url": "{{CLOUD_HOST}}/ipvd",
                            "new_window": true,
                            "icon": "",
                            "authentication": "both",
                            "order": 1,
                            "enabled": true
                        }
                    ]
                },
                {
                    "name": "For Developers",
                    "display_name": "For Developers",
                    "url": "",
                    "new_window": false,
                    "icon": "developers.svg",
                    "authentication": "both",
                    "order": 1,
                    "enabled": true,
                    "nodes": [
                        {
                            "name": "API Documentation",
                            "display_name": "API Documentation",
                            "url": "/api-tool",
                            "newWindow": true,
                            "icon": "",
                            "authentication": "both",
                            "order": 1,
                            "enabled": true
                        },
                        {
                            "name": "Generic Events Generator",
                            "display_name": "Generic Events Generator",
                            "url": "/api-tool/api-createevent-post",
                            "newWindow": true,
                            "icon": "",
                            "authentication": "both",
                            "order": 2,
                            "enabled": true
                        }
                    ]
                },
                {
                    "name": "External Links",
                    "display_name": "External Links",
                    "url": "",
                    "new_window": false,
                    "icon": "external.svg",
                    "authentication": "both",
                    "order": 2,
                    "enabled": true,
                    "nodes": [
                        {
                            "name": "Support",
                            "display_name": "Support",
                            "url": "%SUPPORT_LINK%",
                            "new_window": true,
                            "icon": "",
                            "authentication": "both",
                            "order": 1,
                            "enabled": true
                        }
                    ]
                }
            ]
        },
        {
            "name": "footer",
            "nodes": [
                {
                    "name": "Download %VMS_NAME%",
                    "url": "{{CLOUD_HOST}}/download",
                    "newWindow": false,
                    "enabled": true
                },
                {
                    "name": "Support",
                    "url": "%SUPPORT_LINK%",
                    "newWindow": true,
                    "enabled": true
                },
                {
                    "name": "API Documentation",
                    "url": "/#/api-tool",
                    "newWindow": false,
                    "enabled": true
                },
                {
                    "name": "Download SDK",
                    "url": "{{CLOUD_HOST}}/download/sdk",
                    "newWindow": true,
                    "enabled": true
                }
            ]
        }
    ],
    "copyrightYear": "2020",
    "licenseTypes": [
        {
            "name": "time",
            "title": "Time",
            "deactivationsAllowed": 3
        },
        {
            "name": "trial",
            "title": "Trial",
            "deactivationsAllowed": 0
        },
        {
            "name": "digital",
            "title": "Professional",
            "deactivationsAllowed": 3
        },
        {
            "name": "analog",
            "title": "Analog",
            "deactivationsAllowed": 3
        },
        {
            "name": "edge",
            "title": "Edge",
            "deactivationsAllowed": 3
        },
        {
            "name": "vmax",
            "title": "VMAX",
            "deactivationsAllowed": 3
        },
        {
            "name": "videowall",
            "title": "Video Wall",
            "deactivationsAllowed": 3
        },
        {
            "name": "analogencoder",
            "title": "Analog Encoder",
            "deactivationsAllowed": 3
        },
        {
            "name": "starter",
            "title": "Starter",
            "deactivationsAllowed": 3
        },
        {
            "name": "iomodule",
            "title": "IO Module",
            "deactivationsAllowed": 3
        },
        {
            "name": "bridge",
            "title": "Bridge",
            "deactivationsAllowed": 3
        },
        {
            "name": "nvr",
            "title": "NVR",
            "deactivationsAllowed": 0
        }
    ]
}