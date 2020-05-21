// Clay Config: see https://github.com/pebble/clay
module.exports = [
  {
    "type": "heading",
    "defaultValue": "Settings"
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Theme settings"
      },
      {
        "type":"toggle",
        "messageKey":"Invert",
        "label":"Invert Picture?",
        "defaultValue":false,
      },
      {
        "type": "color",
        "messageKey": "Back1Color",
        "defaultValue": "0xFFFFFF",
        "label": "Background Colour",
        "allowGray":true
      },
      {
        "type": "color",
        "messageKey": "HourColor",
        "defaultValue": "0x000000",
        "label": "Time Colour",
        "allowGray":true
      },
      {
        "type": "color",
        "messageKey": "MinColor",
        "defaultValue": "0x000000",
        "label": "Date & Battery colour",
        "allowGray":true
      },
      {
        "type": "color",
        "messageKey": "Text1Color",
        "defaultValue": "0x000000",
        "label": "Text Colour",
        "allowGray":true,
      },
      {
        "type": "color",
        "messageKey": "Text2Color",
        "defaultValue": "0x000000",
        "label": "Underline Colour",
        "allowGray":true
      },
      {
        "type": "color",
        "messageKey": "Text3Color",
        "defaultValue": "0x000000",
        "label": "Quiet Time & BT Icon Colour",
        "allowGray":true
      },
      {"type": "section",
       "items": [
         {
           "type": "heading",
           "defaultValue": "Night Theme",
           "size":4
         } ,
         {
           "type": "toggle",
           "messageKey": "NightTheme",
           "label": "Activate Night Theme",
           "defaultValue": false,
         },
      {
        "type":"toggle",
        "messageKey":"InvertNight",
        "label":"Invert Picture?",
        "defaultValue":false,
      },
      {
        "type": "color",
        "messageKey": "Back1ColorN",
        "defaultValue": "0xFFFFFF",
        "label": "Background Colour",
        "allowGray":true
      },
      {
        "type": "color",
        "messageKey": "HourColorN",
        "defaultValue": "0x000000",
        "label": "Time Colour",
        "allowGray":true
      },
      {
        "type": "color",
        "messageKey": "MinColorN",
        "defaultValue": "0x000000",
        "label": "Date & Battery Colour",
        "allowGray":true
      },
      {
        "type": "color",
        "messageKey": "Text1ColorN",
        "defaultValue": "0x000000",
        "label": "Text Colour",
        "allowGray":true,
        },
        {
          "type": "color",
          "messageKey": "Text2ColorN",
          "defaultValue": "0x000000",
          "label": "Underline Colour",
          "allowGray":true
        },
         {
           "type": "color",
           "messageKey": "Text3ColorN",
           "defaultValue": "0x000000",
           "label": "Quiet Time & BT Icon Colour",
           "allowGray":true
         },
       ]
         }
       ]
      },
      {
          "type": "submit",
          "defaultValue":"SAVE"
          },
          {
          "type": "heading",
          "defaultValue": "version v1.0",
          "size":6
          },
          {
          "type": "heading",
          "defaultValue": "Made in UK",
          "size":6
          }
        ];
