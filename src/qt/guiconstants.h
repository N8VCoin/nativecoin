// Copyright (c) 2011-2014 The Bitcoin developers
// Copyright (c) 2014-2015 The Dash developers
// Copyright (c) 2015-2018 The nativecoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_QT_GUICONSTANTS_H
#define BITCOIN_QT_GUICONSTANTS_H

/* Milliseconds between model updates */
static const int MODEL_UPDATE_DELAY = 1000;

/* AskPassphraseDialog -- Maximum passphrase length */
static const int MAX_PASSPHRASE_SIZE = 1024;

/* nativecoin GUI -- Size of icons in status bar */
static const int STATUSBAR_ICONSIZE = 26;

static const bool DEFAULT_SPLASHSCREEN = true;

#define QICON_PATH_OVERVIEW             ":/icons/overview"
#define QICON_PATH_SEND                 ":/icons/send"
#define QICON_PATH_RECEIVE              ":/icons/receiving_addresses"
#define QICON_PATH_HISTORY              ":/icons/history"
#define QICON_PATH_MASTERNODES          ":/icons/masternodes"
#define QICON_PATH_QUIT                 ":/icons/quit"
#define QICON_PATH_QT                   ":/qt-project.org/qmessagebox/images/qtlogo-64.png"
#define QICON_PATH_OPTIONS              ":/icons/options"
#define QICON_PATH_ENCRYPTED            ":/icons/lock_closed"
#define QICON_PATH_BACKUP               ":/icons/filesave"
#define QICON_PATH_PASSPHRASE           ":/icons/key"
#define QICON_PATH_SIGNMESSAGE          ":/icons/edit"
#define QICON_PATH_VERIFYMESSAGE        ":/icons/transaction_0"
#define QICON_PATH_BIP38                ":/icons/key"
#define QICON_PATH_MULTISEND            ":/icons/edit"
#define QICON_PATH_CONSOLE              ":/icons/debugwindow"
#define QICON_PATH_NETWORK              ":/icons/networkMonitor"
#define QICON_PATH_PEERS                ":/icons/peers"
#define QICON_PATH_REPAIR               ":/icons/options"
#define QICON_PATH_WALLETCONF           ":/icons/walletConfig"
#define QICON_PATH_MNCONF               ":/icons/mnconfig"
#define QICON_PATH_SHOWBACKUPS          ":/icons/browse"
#define QICON_PATH_SENDADDRESSES        ":/icons/address-book"
#define QICON_PATH_RECEIVEADDRESSES     ":/icons/address-book"
#define QICON_PATH_MULTISIGCREATE       ":/icons/address-book"
#define QICON_PATH_MULTISIGSPEND        ":/icons/send"
#define QICON_PATH_MULTISIGSIGN         ":/icons/editpaste"
#define QICON_PATH_FILEOPEN             ":/icons/editpaste"
#define QICON_PATH_EXPLORER             ":/icons/explorer"
#define QICON_PATH_CONNECTIONZERO       ":/icons/disconnected"
#define QICON_PATH_CONNECTIONGTONE      ":/icons/connect_0"
#define QICON_PATH_CONNECTIONGTTHREE    ":/icons/connect_1"
#define QICON_PATH_CONNECTIONGTSIX      ":/icons/connect_2"
#define QICON_PATH_CONNECTIONGTNINE     ":/icons/connect_3"
#define QICON_PATH_CONNECTIONDEFAULT    ":/icons/connect_4"
#define QICON_PATH_SYNCED               ":/icons/synced"
#define QICON_PATH_SPINNERGENERIC       ":/movies/spinner-%1"
#define QICON_PATH_STAKINGACTIVE        ":/icons/staking_active"
#define QICON_PATH_STAKINGINACTIVE      ":/icons/staking_inactive"
#define QICON_PATH_UNENCRYPTED          ":/icons/lock_open"
#define QICON_PATH_TOR                  ":/icons/onion"
#define QICON_PATH_UNITGENERIC          ":/icons/unit_"
#define QICON_PATH_UNITGENERICT         ":/icons/unit_t"
#define QICON_PATH_COINLOCK             ":/icons/lock_closed"
#define QICON_PATH_INTROWINDOWICON      ":/icons/bitcoin"
#define QICON_PATH_ADDRESSBOOK          ":/icons/address-book"
#define QICON_PATH_ADDRESSPASTE         ":/icons/editpaste"
#define QICON_PATH_ADDRESSCOPY          ":/icons/editcopy"
#define QICON_PATH_ADDRESSDELETE        ":/icons/remove"
#define QICON_PATH_ADDRESSADD           ":/icons/add"
#define QICON_PATH_INPUTDELETE          ":/icons/remove"
#define QICON_PATH_DESTINATIONDELETE    ":/icons/remove"
#define QICON_PATH_KEYDELETE            ":/icons/remove"
#define QICON_PATH_CONSOLEINPUT         ":/icons/tx_input"
#define QICON_PATH_CONSOLEOUTPUT        ":/icons/tx_output"
#define QICON_PATH_CONSOLEERROR         ":/icons/tx_output"
#define QICON_PATH_CONSOLEMISC          ":/icons/tx_inout"
#define QICON_PATH_DEBUGLOG             ":/icons/export"
#define QICON_PATH_STAKEREWARD          ":/icons/stakeReward"
#define QICON_PATH_MNREWARD             ":/icons/mnReward"
#define QICON_PATH_TXIN                 ":/icons/tx_input"
#define QICON_PATH_TXOUT                ":/icons/tx_output"
#define QICON_PATH_TXINOUT              ":/icons/tx_inout"
#define QICON_PATH_TXUNCONFIRMED        ":/icons/transaction_0"
#define QICON_PATH_TXCONFIRMONE         ":/icons/transaction_1"
#define QICON_PATH_TXCONFIRMTWO         ":/icons/transaction_2"
#define QICON_PATH_TXCONFIRMTHREE       ":/icons/transaction_3"
#define QICON_PATH_TXCONFIRMFOUR        ":/icons/transaction_4"
#define QICON_PATH_TXCONFIRMDEFAULT     ":/icons/transaction_5"
#define QICON_PATH_TXCONFIRMED          ":/icons/transaction_confirmed"
#define QICON_PATH_TXCONFLICTED         ":/icons/transaction_conflicted"
#define QICON_PATH_TXNOACCEPT           ":/icons/transaction_0"
#define QICON_PATH_TXIMMATURE           ":/icons/transaction_%1"
#define QICON_PATH_TXWATCHADDRESS       ":/icons/eye"
#define QICON_PATH_TXWATCHONLYYES       ":/icons/eye_plus"
#define QICON_PATH_TXWATCHONLYNO        ":/icons/eye_minus"
#define QICON_PATH_EXPORTDATA           ":/icons/export"
#define QIMAGE_PATH_SPLASH              ":/images/splash"

#define DEFAULT_WINDOW_SIZE QSize(850, 550)

/* Invalid field background style */
#define STYLE_INVALID "background:#FF8080"

/* Transaction list -- unconfirmed transaction */
#define COLOR_UNCONFIRMED QColor(91, 76, 134)
/* Transaction list -- negative amount */
#define COLOR_NEGATIVE QColor(206, 0, 188)
/* Transaction list -- bare address (without label) */
#define COLOR_BAREADDRESS QColor(140, 140, 140)
/* Transaction list -- TX status decoration - open until date */
#define COLOR_TX_STATUS_OPENUNTILDATE QColor(64, 64, 255)
/* Transaction list -- TX status decoration - offline */
#define COLOR_TX_STATUS_OFFLINE QColor(192, 192, 192)
/* Transaction list -- TX status decoration - default color */
#define COLOR_BLACK QColor(211, 211, 211)
/* Transaction list -- TX status decoration - conflicted */
#define COLOR_CONFLICTED QColor(255, 0, 0)
/* Transaction list -- TX status decoration - orphan (Light Gray #D3D3D3) */
#define COLOR_ORPHAN QColor(211, 211, 211)
/* Transaction list -- TX status decoration - stake (BlueViolet #8A2BE2) */
#define COLOR_STAKE QColor(242, 242, 242)
/* Tooltips longer than this (in characters) are converted into rich text,
   so that they can be word-wrapped.
 */
static const int TOOLTIP_WRAP_THRESHOLD = 80;

/* Maximum allowed URI length */
static const int MAX_URI_LENGTH = 255;

/* QRCodeDialog -- size of exported QR Code image */
#define EXPORT_IMAGE_SIZE 256

/* Number of frames in spinner animation */
#define SPINNER_FRAMES 36

#define QAPP_ORG_NAME "NativeCoin"
#define QAPP_ORG_DOMAIN "n8vcoin.io"
#define QAPP_APP_NAME_DEFAULT "nativecoin-Qt"
#define QAPP_APP_NAME_TESTNET "nativecoin-Qt-testnet"

#endif // BITCOIN_QT_GUICONSTANTS_H
