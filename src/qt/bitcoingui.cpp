// Copyright (c) 2011-2014 The Bitcoin developers
// Copyright (c) 2014-2015 The Dash developers
// Copyright (c) 2015-2018 The nativecoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "bitcoingui.h"

#include "bitcoinunits.h"
#include "clientmodel.h"
#include "guiconstants.h"
#include "guiutil.h"
#include "miner.h"
#include "networkstyle.h"
#include "notificator.h"
#include "openuridialog.h"
#include "optionsdialog.h"
#include "optionsmodel.h"
#include "rpcconsole.h"
#include "utilitydialog.h"

#ifdef ENABLE_WALLET
#include "blockexplorer.h"
#include "walletframe.h"
#include "walletmodel.h"
#endif // ENABLE_WALLET

#ifdef Q_OS_MAC
#include "macdockiconhandler.h"
#endif

#include "init.h"
#include "masternodelist.h"
#include "ui_interface.h"
#include "util.h"

#include <iostream>

#include <QAction>
#include <QApplication>
#include <QDateTime>
#include <QDesktopWidget>
#include <QDragEnterEvent>
#include <QIcon>
#include <QListWidget>
#include <QMenuBar>
#include <QMessageBox>
#include <QMimeData>
#include <QProgressBar>
#include <QProgressDialog>
#include <QSettings>
#include <QStackedWidget>
#include <QStatusBar>
#include <QStyle>
#include <QTimer>
#include <QToolBar>
#include <QUrlQuery>
#include <QVBoxLayout>

const QString BitcoinGUI::DEFAULT_WALLET = "~Default";

BitcoinGUI::BitcoinGUI(const NetworkStyle* networkStyle, QWidget* parent) : QMainWindow(parent),
                                                                            clientModel(0),
                                                                            walletFrame(0),
                                                                            unitDisplayControl(0),
                                                                            labelStakingIcon(0),
                                                                            labelEncryptionIcon(0),
                                                                            labelTorIcon(0),
                                                                            labelConnectionsIcon(0),
                                                                            labelBlocksIcon(0),
                                                                            progressBarLabel(0),
                                                                            progressBar(0),
                                                                            progressDialog(0),
                                                                            appMenuBar(0),
                                                                            overviewAction(0),
                                                                            historyAction(0),
                                                                            masternodeAction(0),
                                                                            quitAction(0),
                                                                            sendCoinsAction(0),
                                                                            usedSendingAddressesAction(0),
                                                                            usedReceivingAddressesAction(0),
                                                                            signMessageAction(0),
                                                                            verifyMessageAction(0),
                                                                            bip38ToolAction(0),
                                                                            multisigCreateAction(0),
                                                                            multisigSpendAction(0),
                                                                            multisigSignAction(0),
                                                                            aboutAction(0),
                                                                            receiveCoinsAction(0),
                                                                            governanceAction(0),
                                                                            optionsAction(0),
                                                                            toggleHideAction(0),
                                                                            encryptWalletAction(0),
                                                                            backupWalletAction(0),
                                                                            changePassphraseAction(0),
                                                                            aboutQtAction(0),
                                                                            openRPCConsoleAction(0),
                                                                            openAction(0),
                                                                            showHelpMessageAction(0),
                                                                            multiSendAction(0),
                                                                            trayIcon(0),
                                                                            trayIconMenu(0),
                                                                            notificator(0),
                                                                            rpcConsole(0),
                                                                            explorerWindow(0),
                                                                            prevBlocks(0),
                                                                            spinnerFrame(0)
{
    /* Open CSS when configured */
    this->setStyleSheet(GUIUtil::loadStyleSheet());

    GUIUtil::restoreWindowGeometry("nWindow", DEFAULT_WINDOW_SIZE, this);

    QString windowTitle = tr("NativeCoin Core") + " - ";
#ifdef ENABLE_WALLET
    /* if compiled with wallet support, -disablewallet can still disable the wallet */
    enableWallet = !GetBoolArg("-disablewallet", false);
#else
    enableWallet = false;
#endif // ENABLE_WALLET
    if (enableWallet) {
        windowTitle += tr("Wallet");
    } else {
        windowTitle += tr("Node");
    }
    QString userWindowTitle = QString::fromStdString(GetArg("-windowtitle", ""));
    if (!userWindowTitle.isEmpty()) windowTitle += " - " + userWindowTitle;
    windowTitle += " " + networkStyle->getTitleAddText();
#ifndef Q_OS_MAC
    QApplication::setWindowIcon(networkStyle->getAppIcon());
    setWindowIcon(networkStyle->getAppIcon());
#else
    MacDockIconHandler::instance()->setIcon(networkStyle->getAppIcon());
#endif
    setWindowTitle(windowTitle);

    rpcConsole = new RPCConsole(enableWallet ? this : 0);
#ifdef ENABLE_WALLET
    if (enableWallet) {
        /** Create wallet frame*/
        walletFrame     = new WalletFrame(this);
        explorerWindow  = new BlockExplorer(this);
    } else
#endif // ENABLE_WALLET
    {
        /* When compiled without wallet or -disablewallet is provided,
         * the central widget is the rpc console.
         */
        setCentralWidget(rpcConsole);
    }
    // Accept D&D of URIs
    setAcceptDrops(true);
    // Create actions for the toolbar, menu bar and tray/dock icon
    // Needs walletFrame to be initialized
    createActions(networkStyle);
    // Create application menu bar
    createMenuBar();
    // Create the toolbars
    createToolBars();
    // Create system tray icon and notification
    createTrayIcon(networkStyle);
    // Create status bar
    statusBar();

    // Status bar notification icons
    QFrame* frameBlocks             = new QFrame();
    QHBoxLayout* frameBlocksLayout  = new QHBoxLayout(frameBlocks);
    unitDisplayControl              = new UnitDisplayStatusBarControl();
    labelStakingIcon                = new QLabel();
    labelEncryptionIcon             = new QLabel();
    labelTorIcon                    = new QLabel();
    labelConnectionsIcon            = new QLabel(); // Change to QLabel to match others
    labelBlocksIcon                 = new QLabel();
    progressBarLabel                = new QLabel();
    progressBar                     = new GUIUtil::ProgressBar();
    // Set Object Names
    labelStakingIcon                -> setObjectName("labelStakingIcon");
    labelEncryptionIcon             -> setObjectName("labelEncryptionIcon");
    labelConnectionsIcon            -> setObjectName("labelConnectionsIcon");
    progressBarLabel                -> setObjectName("progressBarLabel");
    // Formatting
    frameBlocks                     -> setContentsMargins(0, 0, 0, 0);
    frameBlocks                     -> setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    frameBlocksLayout               -> setAlignment(Qt::AlignBottom);
    frameBlocks                     -> setStyleSheet("{background-color:rgba(88,88,88,0)}");
    frameBlocksLayout               -> setContentsMargins(15, 0, 10, 0);
    frameBlocksLayout               -> setSpacing(10);
    
#ifdef ENABLE_WALLET
    if (enableWallet) {
        frameBlocksLayout           -> addStretch();
        frameBlocksLayout           -> addWidget(unitDisplayControl);
        frameBlocksLayout           -> addStretch();
        frameBlocksLayout           -> addWidget(labelEncryptionIcon);
        frameBlocksLayout           -> addStretch();
        frameBlocksLayout           -> addWidget(labelStakingIcon);
        frameBlocksLayout           -> addStretch();
    }
#endif // ENABLE_WALLET
    frameBlocksLayout               -> addWidget(labelTorIcon);
    frameBlocksLayout               -> addStretch();
    frameBlocksLayout               -> addWidget(labelConnectionsIcon);
    frameBlocksLayout               -> addStretch();
    frameBlocksLayout               -> addWidget(labelBlocksIcon);
    frameBlocksLayout               -> addStretch();
    // Progress bar and label for blocks download
    progressBarLabel                -> setVisible(true);
    progressBarLabel                -> setAlignment(Qt::AlignCenter | Qt::AlignBottom);
    progressBar                     -> setAlignment(Qt::AlignCenter | Qt::AlignBottom);
    progressBar                     -> setVisible(true);
    // Override style sheet for progress bar for styles that have a segmented progress bar,
    // as they make the text unreadable (workaround for issue #1071)
    // See https://doc.qt.io/qt-5/gallery.html
    QString curStyle = QApplication::style()->metaObject()->className();
    if (curStyle == "QWindowsStyle" || curStyle == "QWindowsXPStyle") {
        progressBar->setStyleSheet("QProgressBar { background-color: #F8F8F8; border: 1px solid grey; border-radius: 7px; padding: 1px; text-align: center; } QProgressBar::chunk { background: QLinearGradient(x1: 0, y1: 0, x2: 1, y2: 0, stop: 0 #00CCFF, stop: 1 #33CCFF); border-radius: 7px; margin: 0px; }");
    }
    statusBar()                     -> addWidget(progressBarLabel);
    statusBar()                     -> addWidget(progressBar);
    statusBar()                     -> addPermanentWidget(frameBlocks);
    // Jump directly to tabs in RPC-console --------------------------------------------------------------------------------------
    connect(openInfoAction,             SIGNAL(triggered()),                rpcConsole,     SLOT(showInfo()));
    connect(openRPCConsoleAction,       SIGNAL(triggered()),                rpcConsole,     SLOT(showConsole()));
    connect(openNetworkAction,          SIGNAL(triggered()),                rpcConsole,     SLOT(showNetwork()));
    connect(openPeersAction,            SIGNAL(triggered()),                rpcConsole,     SLOT(showPeers()));
    connect(openRepairAction,           SIGNAL(triggered()),                rpcConsole,     SLOT(showRepair()));
    connect(openConfEditorAction,       SIGNAL(triggered()),                rpcConsole,     SLOT(showConfEditor()));
    connect(openMNConfEditorAction,     SIGNAL(triggered()),                rpcConsole,     SLOT(showMNConfEditor()));
    connect(showBackupsAction,          SIGNAL(triggered()),                rpcConsole,     SLOT(showBackups()));
    connect(labelConnectionsIcon,       SIGNAL(clicked()),                  rpcConsole,     SLOT(showPeers()));
    connect(labelEncryptionIcon,        SIGNAL(clicked()),                  walletFrame,    SLOT(toggleLockWallet()));
    // Get restart command-line parameters and handle restart
    connect(rpcConsole,                 SIGNAL(handleRestart(QStringList)), this,           SLOT(handleRestart(QStringList)));
    // prevents an open debug window from becoming stuck/unusable on client shutdown
    connect(quitAction,                 SIGNAL(triggered()),                rpcConsole,     SLOT(hide()));
    connect(openBlockExplorerAction,    SIGNAL(triggered()),                explorerWindow, SLOT(show()));
    // prevents an open debug window from becoming stuck/unusable on client shutdown
    connect(quitAction,                 SIGNAL(triggered()),                explorerWindow, SLOT(hide()));
    //----------------------------------------------------------------------------------------------------------------------------
    // Install event filter to be able to catch status tip events (QEvent::StatusTip)
    this->installEventFilter(this);
    //----------------------------------------------------------------------------------------------------------------------------
    // Initially wallet actions should be disabled
    setWalletActionsEnabled(false);
    //----------------------------------------------------------------------------------------------------------------------------
    // Subscribe to notifications from core
    subscribeToCoreSignals();
    //----------------------------------------------------------------------------------------------------------------------------
    QTimer* timerStakingIcon = new QTimer(labelStakingIcon);
    connect(timerStakingIcon, SIGNAL(timeout()), this, SLOT(setStakingStatus()));
    timerStakingIcon->start(10000);
    setStakingStatus();
} // END BitcoinGUI

BitcoinGUI::~BitcoinGUI()
{
    // Unsubscribe from notifications from core
    unsubscribeFromCoreSignals();

    GUIUtil::saveWindowGeometry("nWindow", this);
    if (trayIcon) // Hide tray icon, as deleting will let it linger until quit (on Ubuntu)
        trayIcon->hide();
#ifdef Q_OS_MAC
    delete appMenuBar;
    MacDockIconHandler::cleanup();
#endif
}

void BitcoinGUI::createActions(const NetworkStyle* networkStyle)
{
    // Object Definitions and Instantiation
    QActionGroup* tabGroup  = new QActionGroup(this);
    overviewAction          = new QAction(QIcon(QICON_PATH_OVERVIEW),   tr("HOME"),    this);
    sendCoinsAction         = new QAction(QIcon(QICON_PATH_SEND),       tr("SEND"),    this);
    receiveCoinsAction      = new QAction(QIcon(QICON_PATH_RECEIVE),    tr("RECEIVE"),    this);
    historyAction           = new QAction(QIcon(QICON_PATH_HISTORY),    tr("HISTORY"),    this);
    // Add the buttons to the tab group
    tabGroup                -> addAction(overviewAction);
    tabGroup                -> addAction(sendCoinsAction);
    tabGroup                -> addAction(receiveCoinsAction);
    tabGroup                -> addAction(historyAction);
    // Define the status tips
    overviewAction          -> setStatusTip(tr("Show general overview of wallet"));
    sendCoinsAction         -> setStatusTip(tr("Send coins to a nativecoin address"));
    receiveCoinsAction      -> setStatusTip(tr("Request payments (generates QR codes and NativeCoin: URIs)"));
    historyAction           -> setStatusTip(tr("Browse transaction history"));
    // Set tool tips
    overviewAction          -> setToolTip(overviewAction        -> statusTip());
    sendCoinsAction         -> setToolTip(sendCoinsAction       -> statusTip());
    receiveCoinsAction      -> setToolTip(receiveCoinsAction    -> statusTip());
    historyAction           -> setToolTip(historyAction         -> statusTip());
    // Make the buttons able to be "selected"
    overviewAction          -> setCheckable(true);
    sendCoinsAction         -> setCheckable(true);
    receiveCoinsAction      -> setCheckable(true);
    historyAction           -> setCheckable(true);
    // Shortcut Key Definitions
#ifdef Q_OS_MAC
    overviewAction          -> setShortcut(QKeySequence(Qt::CTRL + Qt::Key_1));
    sendCoinsAction         -> setShortcut(QKeySequence(Qt::CTRL + Qt::Key_2));
    receiveCoinsAction      -> setShortcut(QKeySequence(Qt::CTRL + Qt::Key_3));
    historyAction           -> setShortcut(QKeySequence(Qt::CTRL + Qt::Key_4));
#else
    overviewAction          -> setShortcut(QKeySequence(Qt::ALT + Qt::Key_1));
    sendCoinsAction         -> setShortcut(QKeySequence(Qt::ALT + Qt::Key_2));
    receiveCoinsAction      -> setShortcut(QKeySequence(Qt::ALT + Qt::Key_3));
    historyAction           -> setShortcut(QKeySequence(Qt::ALT + Qt::Key_4));
#endif

#ifdef ENABLE_WALLET
    QSettings settings;
    if (settings.value("fShowMasternodesTab").toBool()) {
        masternodeAction    =  new QAction(QIcon(QICON_PATH_MASTERNODES).pixmap(QSize(48,60)), tr("MNODES"),   this);
        masternodeAction    -> setStatusTip(tr("Browse masternodes"));
        masternodeAction    -> setToolTip(masternodeAction -> statusTip());
        masternodeAction    -> setCheckable(true);
        masternodeAction    -> setObjectName("masternodesButton");
    #ifdef Q_OS_MAC
        masternodeAction    -> setShortcut(QKeySequence(Qt::CTRL + Qt::Key_6));
    #else
        masternodeAction    -> setShortcut(QKeySequence(Qt::ALT + Qt::Key_6));
    #endif
        tabGroup->addAction(masternodeAction);
        connect(masternodeAction,   SIGNAL(triggered()), this, SLOT(showNormalIfMinimized()));
        connect(masternodeAction,   SIGNAL(triggered()), this, SLOT(gotoMasternodePage()));
    }
    // These showNormalIfMinimized are needed because Send Coins and Receive Coins
    // can be triggered from the tray menu, and need to show the GUI to be useful.
    connect(overviewAction,         SIGNAL(triggered()), this, SLOT(showNormalIfMinimized()));
    connect(overviewAction,         SIGNAL(triggered()), this, SLOT(gotoOverviewPage()));
    connect(sendCoinsAction,        SIGNAL(triggered()), this, SLOT(showNormalIfMinimized()));
    connect(sendCoinsAction,        SIGNAL(triggered()), this, SLOT(gotoSendCoinsPage()));
    connect(receiveCoinsAction,     SIGNAL(triggered()), this, SLOT(showNormalIfMinimized()));
    connect(receiveCoinsAction,     SIGNAL(triggered()), this, SLOT(gotoReceiveCoinsPage()));
    connect(historyAction,          SIGNAL(triggered()), this, SLOT(showNormalIfMinimized()));
    connect(historyAction,          SIGNAL(triggered()), this, SLOT(gotoHistoryPage()));
#endif // END IF ENABLE_WALLET
    //------------------------------------------------------------------------------------------------------------//
    quitAction                      = new QAction(QIcon(QICON_PATH_QUIT),               tr("E&xit"),                                this);
    aboutAction                     = new QAction(networkStyle->getAppIcon(),           tr("&About NativeCoin Core"),               this);
    aboutQtAction                   = new QAction(QIcon(QICON_PATH_QT),                 tr("About &Qt"),                            this);
    optionsAction                   = new QAction(QIcon(QICON_PATH_OPTIONS),            tr("&Options..."),                          this);
    toggleHideAction                = new QAction(networkStyle->getAppIcon(),           tr("&Show / Hide"),                         this);
    encryptWalletAction             = new QAction(QIcon(QICON_PATH_ENCRYPTED),          tr("&Encrypt Wallet..."),                   this);
    backupWalletAction              = new QAction(QIcon(QICON_PATH_BACKUP),             tr("&Backup Wallet..."),                    this);
    changePassphraseAction          = new QAction(QIcon(QICON_PATH_PASSPHRASE),         tr("&Change Passphrase..."),                this);
    unlockWalletAction              = new QAction(                                      tr("&Unlock Wallet..."),                    this);
    lockWalletAction                = new QAction(                                      tr("&Lock Wallet"),                         this);
    signMessageAction               = new QAction(QIcon(QICON_PATH_SIGNMESSAGE),        tr("Sign &message..."),                     this);
    verifyMessageAction             = new QAction(QIcon(QICON_PATH_VERIFYMESSAGE),      tr("&Verify message..."),                   this);
    bip38ToolAction                 = new QAction(QIcon(QICON_PATH_BIP38),              tr("&BIP38 tool"),                          this);
    multiSendAction                 = new QAction(QIcon(QICON_PATH_MULTISEND),          tr("&MultiSend"),                           this);
    openInfoAction                  = new QAction(QApplication::style()->standardIcon(QStyle::SP_MessageBoxInformation), tr("&Information"), this);
    openRPCConsoleAction            = new QAction(QIcon(QICON_PATH_CONSOLE),            tr("&Debug console"),                       this);
    openNetworkAction               = new QAction(QIcon(QICON_PATH_NETWORK),            tr("&Network Monitor"),                     this);
    openPeersAction                 = new QAction(QIcon(QICON_PATH_PEERS),              tr("&Peers list"),                          this);
    openRepairAction                = new QAction(QIcon(QICON_PATH_REPAIR),             tr("Wallet &Repair"),                       this);
    openConfEditorAction            = new QAction(QIcon(QICON_PATH_WALLETCONF),         tr("Open Wallet &Configuration File"),      this);
    openMNConfEditorAction          = new QAction(QIcon(QICON_PATH_MNCONF),             tr("Open &Masternode Configuration File"),  this);
    showBackupsAction               = new QAction(QIcon(QICON_PATH_SHOWBACKUPS),        tr("Show Automatic &Backups"),              this);
    usedSendingAddressesAction      = new QAction(QIcon(QICON_PATH_SENDADDRESSES),      tr("&Sending addresses..."),                this);
    usedReceivingAddressesAction    = new QAction(QIcon(QICON_PATH_RECEIVEADDRESSES),   tr("&Receiving addresses..."),              this);
    multisigCreateAction            = new QAction(QIcon(QICON_PATH_MULTISIGCREATE),     tr("&Multisignature creation..."),          this);
    multisigSpendAction             = new QAction(QIcon(QICON_PATH_MULTISIGSPEND),      tr("&Multisignature spending..."),          this);
    multisigSignAction              = new QAction(QIcon(QICON_PATH_MULTISIGSIGN),       tr("&Multisignature signing..."),           this);
    openAction                      = new QAction(QIcon(QICON_PATH_FILEOPEN),           tr("Open &URI..."),                         this);
    openBlockExplorerAction         = new QAction(QIcon(QICON_PATH_EXPLORER),           tr("&Blockchain explorer"),                 this);
    showHelpMessageAction           = new QAction(QApplication::style()->standardIcon(QStyle::SP_MessageBoxInformation), tr("&Command-line options"), this);


    quitAction                      ->setStatusTip( tr("Quit application"));
    aboutAction                     ->setStatusTip( tr("Show information about NativeCoin Core"));
    aboutQtAction                   ->setStatusTip( tr("Show information about Qt"));
    optionsAction                   ->setStatusTip( tr("Modify configuration options for NativeCoin"));
    toggleHideAction                ->setStatusTip( tr("Show or hide the main Window"));
    encryptWalletAction             ->setStatusTip( tr("Encrypt the private keys that belong to your wallet"));
    backupWalletAction              ->setStatusTip( tr("Backup wallet to another location"));
    changePassphraseAction          ->setStatusTip( tr("Change the passphrase used for wallet encryption"));
    unlockWalletAction              ->setToolTip(   tr("Unlock wallet"));
    signMessageAction               ->setStatusTip( tr("Sign messages with your nativecoin addresses to prove you own them"));
    verifyMessageAction             ->setStatusTip( tr("Verify messages to ensure they were signed with specified nativecoin addresses"));
    bip38ToolAction                 ->setToolTip(   tr("Encrypt and decrypt private keys using a passphrase"));
    multiSendAction                 ->setToolTip(   tr("MultiSend Settings"));
    openInfoAction                  ->setStatusTip( tr("Show diagnostic information"));
    openRPCConsoleAction            ->setStatusTip( tr("Open debugging console"));
    openNetworkAction               ->setStatusTip( tr("Show network monitor"));
    openPeersAction                 ->setStatusTip( tr("Show peers info"));
    openRepairAction                ->setStatusTip( tr("Show wallet repair options"));
    openConfEditorAction            ->setStatusTip( tr("Open configuration file"));
    openMNConfEditorAction          ->setStatusTip( tr("Open Masternode configuration file"));
    showBackupsAction               ->setStatusTip( tr("Show automatically created wallet backups"));
    usedSendingAddressesAction      ->setStatusTip( tr("Show the list of used sending addresses and labels"));
    usedReceivingAddressesAction    ->setStatusTip( tr("Show the list of used receiving addresses and labels"));
    multisigCreateAction            ->setStatusTip( tr("Create a new multisignature address and add it to this wallet"));
    multisigSpendAction             ->setStatusTip( tr("Spend from a multisignature address"));
    multisigSignAction              ->setStatusTip( tr("Sign with a multisignature address"));
    openAction                      ->setStatusTip( tr("Open a nativecoin: URI or payment request"));
    openBlockExplorerAction         ->setStatusTip( tr("Block explorer window"));
    showHelpMessageAction           ->setStatusTip( tr("Show the NativeCoin Core help message to get a list with possible nativecoin command-line options"));

    quitAction                      ->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));

    quitAction                      ->setMenuRole(QAction::QuitRole);
    aboutAction                     ->setMenuRole(QAction::AboutRole);
    aboutQtAction                   ->setMenuRole(QAction::AboutQtRole);
    optionsAction                   ->setMenuRole(QAction::PreferencesRole);
    showHelpMessageAction           ->setMenuRole(QAction::NoRole);

    encryptWalletAction             ->setCheckable(true);
    multiSendAction                 ->setCheckable(true);

    //------------------------------------------------------------------------------------------------------------//
    connect(quitAction,             SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(aboutAction,            SIGNAL(triggered()), this, SLOT(aboutClicked()));
    connect(aboutQtAction,          SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(optionsAction,          SIGNAL(triggered()), this, SLOT(optionsClicked()));
    connect(toggleHideAction,       SIGNAL(triggered()), this, SLOT(toggleHidden()));
    connect(showHelpMessageAction,  SIGNAL(triggered()), this, SLOT(showHelpMessageClicked()));
#ifdef ENABLE_WALLET
    if (walletFrame) {
        connect(encryptWalletAction,            SIGNAL(triggered(bool)),    walletFrame,    SLOT(encryptWallet(bool)));
        connect(backupWalletAction,             SIGNAL(triggered()),        walletFrame,    SLOT(backupWallet()));
        connect(changePassphraseAction,         SIGNAL(triggered()),        walletFrame,    SLOT(changePassphrase()));
        connect(unlockWalletAction,             SIGNAL(triggered(bool)),    walletFrame,    SLOT(unlockWallet(bool)));
        connect(lockWalletAction,               SIGNAL(triggered()),        walletFrame,    SLOT(lockWallet()));
        connect(signMessageAction,              SIGNAL(triggered()),        this,           SLOT(gotoSignMessageTab()));
        connect(verifyMessageAction,            SIGNAL(triggered()),        this,           SLOT(gotoVerifyMessageTab()));
        connect(bip38ToolAction,                SIGNAL(triggered()),        this,           SLOT(gotoBip38Tool()));
        connect(usedSendingAddressesAction,     SIGNAL(triggered()),        walletFrame,    SLOT(usedSendingAddresses()));
        connect(usedReceivingAddressesAction,   SIGNAL(triggered()),        walletFrame,    SLOT(usedReceivingAddresses()));
        connect(openAction,                     SIGNAL(triggered()),        this,           SLOT(openClicked()));
        connect(multiSendAction,                SIGNAL(triggered()),        this,           SLOT(gotoMultiSendDialog()));
        connect(multisigCreateAction,           SIGNAL(triggered()),        this,           SLOT(gotoMultisigCreate()));
        connect(multisigSpendAction,            SIGNAL(triggered()),        this,           SLOT(gotoMultisigSpend()));
        connect(multisigSignAction,             SIGNAL(triggered()),        this,           SLOT(gotoMultisigSign()));
    }
#endif // ENABLE_WALLET
}

void BitcoinGUI::createMenuBar()
{
#ifdef Q_OS_MAC
    // Create a decoupled menu bar on Mac which stays even if the window is closed
    appMenuBar = new QMenuBar();
#else
    // Get the main window's menu bar on other platforms
    appMenuBar = menuBar();
#endif
    // Configure the menus
    //------------FILE Menu------------//
    QMenu* file = appMenuBar->addMenu(tr("&File"));
    if (walletFrame) {
        file->addAction(openAction);
        file->addAction(backupWalletAction);
        file->addAction(signMessageAction);
        file->addAction(verifyMessageAction);
        file->addSeparator();
        file->addAction(usedSendingAddressesAction);
        file->addAction(usedReceivingAddressesAction);
        file->addSeparator();
        file->addAction(multisigCreateAction);
        file->addAction(multisigSpendAction);
        file->addAction(multisigSignAction);
        file->addSeparator();
    }
    file->addAction(quitAction);
    //------------SETTINGS Menu------------//
    QMenu* settings = appMenuBar->addMenu(tr("&Settings"));
    if (walletFrame) {
        settings->addAction(encryptWalletAction);
        settings->addAction(changePassphraseAction);
        settings->addAction(unlockWalletAction);
        settings->addAction(lockWalletAction);
        settings->addAction(bip38ToolAction);
        settings->addAction(multiSendAction);
        settings->addSeparator();
    }
    settings->addAction(optionsAction);
    //------------TOOLS Menu------------//
    if (walletFrame) {
        QMenu* tools = appMenuBar->addMenu(tr("&Tools"));
        tools->addAction(openInfoAction);
        tools->addAction(openRPCConsoleAction);
        tools->addAction(openNetworkAction);
        tools->addAction(openPeersAction);
        tools->addAction(openRepairAction);
        tools->addSeparator();
        tools->addAction(openConfEditorAction);
        tools->addAction(openMNConfEditorAction);
        tools->addAction(showBackupsAction);
        tools->addAction(openBlockExplorerAction);
    }
    //------------HELP Menu------------//
    QMenu* help = appMenuBar->addMenu(tr("&Help"));
    help->addAction(showHelpMessageAction);
    help->addSeparator();
    help->addAction(aboutAction);
    help->addAction(aboutQtAction);
}

void BitcoinGUI::createToolBars()
{
    if (walletFrame) {
        // Import settings
        QSettings settings;
        bool showMNpage = settings.value("fShowMasternodesTab").toBool();
        // Create Objects
        QToolBar* toolbar = new QToolBar(tr("Tabs toolbar"));
        // QAction*  spacer  = new QAction(this);
        // Add some empty space at the top of the toolbars
        // toolbar->addAction(spacer);
        // Add Actions
        toolbar->addAction(overviewAction);
        toolbar->addAction(sendCoinsAction);
        toolbar->addAction(receiveCoinsAction);
        toolbar->addAction(historyAction);
        // If show masternodes is checked then add a button for it
        if (showMNpage) {
            toolbar->addAction(masternodeAction);
        }
        // Format Objects
        toolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        toolbar->setMovable(false); // remove unused icon in upper left corner
        toolbar->setOrientation(Qt::Vertical);
        toolbar->setIconSize(QSize(48,48));
        // Set default selection
        overviewAction->setChecked(true);

        /** Create additional container for toolbar and walletFrame and make it the central widget.
            This is a workaround mostly for toolbar styling on Mac OS but should work fine for every other OSes too.
        */
        QVBoxLayout* layout = new QVBoxLayout;
        layout->addWidget(toolbar);
        layout->addWidget(walletFrame);
        layout->setSpacing(0);
        layout->setContentsMargins(QMargins());
        layout->setDirection(QBoxLayout::LeftToRight);
        QWidget* containerWidget = new QWidget();
        containerWidget->setLayout(layout);
        setCentralWidget(containerWidget);
        // Naming
        toolbar->setObjectName("Main-Toolbar");
        //toolbar->widgetForAction(spacer)->setObjectName("ToolbarSpacer");
    }
}

void BitcoinGUI::setClientModel(ClientModel* clientModel)
{
    this->clientModel = clientModel;
    if (clientModel) {
        // Create system tray menu (or setup the dock menu) that late to prevent users from calling actions,
        // while the client has not yet fully loaded
        createTrayIconMenu();
        // Keep up to date with client
        setNumConnections(clientModel->getNumConnections());
        connect(clientModel, SIGNAL(numConnectionsChanged(int)), this, SLOT(setNumConnections(int)));
        setNumBlocks(clientModel->getNumBlocks());
        connect(clientModel, SIGNAL(numBlocksChanged(int)), this, SLOT(setNumBlocks(int)));
        // Receive and report messages from client model
        connect(clientModel, SIGNAL(message(QString, QString, unsigned int)), this, SLOT(message(QString, QString, unsigned int)));
        // Show progress dialog
        connect(clientModel, SIGNAL(showProgress(QString, int)), this, SLOT(showProgress(QString, int)));
        rpcConsole->setClientModel(clientModel);
        updateTorIcon();
#ifdef ENABLE_WALLET
        if (walletFrame) {
            walletFrame->setClientModel(clientModel);
        }
#endif // ENABLE_WALLET
        unitDisplayControl->setOptionsModel(clientModel->getOptionsModel());
        //Show trayIcon
        if (trayIcon)
        {
          trayIcon->show();
        }
    } else {
        // Disable possibility to show main window via action
        toggleHideAction->setEnabled(false);
        if (trayIconMenu) {
            // Disable context menu on tray icon
            trayIconMenu->clear();
        }
    }
}

#ifdef ENABLE_WALLET
bool BitcoinGUI::addWallet(const QString& name, WalletModel* walletModel)
{
    if (!walletFrame)
        return false;
    setWalletActionsEnabled(true);
    return walletFrame->addWallet(name, walletModel);
}

bool BitcoinGUI::setCurrentWallet(const QString& name)
{
    if (!walletFrame)
        return false;
    return walletFrame->setCurrentWallet(name);
}

void BitcoinGUI::removeAllWallets()
{
    if (!walletFrame)
        return;
    setWalletActionsEnabled(false);
    walletFrame->removeAllWallets();
}
#endif // ENABLE_WALLET

void BitcoinGUI::setWalletActionsEnabled(bool enabled)
{
    overviewAction                  -> setEnabled(enabled);
    sendCoinsAction                 -> setEnabled(enabled);
    receiveCoinsAction              -> setEnabled(enabled);
    historyAction                   -> setEnabled(enabled);
    QSettings settings;
    if (settings.value("fShowMasternodesTab").toBool()) {
        masternodeAction            -> setEnabled(enabled);
    }
    encryptWalletAction             -> setEnabled(enabled);
    backupWalletAction              -> setEnabled(enabled);
    changePassphraseAction          -> setEnabled(enabled);
    signMessageAction               -> setEnabled(enabled);
    verifyMessageAction             -> setEnabled(enabled);
    multisigCreateAction            -> setEnabled(enabled);
    multisigSpendAction             -> setEnabled(enabled);
    multisigSignAction              -> setEnabled(enabled);
    bip38ToolAction                 -> setEnabled(enabled);
    usedSendingAddressesAction      -> setEnabled(enabled);
    usedReceivingAddressesAction    -> setEnabled(enabled);
    openAction                      -> setEnabled(enabled);
}

void BitcoinGUI::createTrayIcon(const NetworkStyle* networkStyle)
{
#ifndef Q_OS_MAC
    trayIcon = new QSystemTrayIcon(this);
    QString toolTip = tr("NativeCoin Core client") + " " + networkStyle->getTitleAddText();
    trayIcon->setToolTip(toolTip);
    trayIcon->setIcon(networkStyle->getAppIcon());
    trayIcon->hide();
#endif
    notificator = new Notificator(QApplication::applicationName(), trayIcon, this);
}

void BitcoinGUI::createTrayIconMenu()
{
#ifndef Q_OS_MAC
    // return if trayIcon is unset (only on non-Mac OSes)
    if (!trayIcon)
        return;
    trayIconMenu = new QMenu(this);
    trayIcon->setContextMenu(trayIconMenu);
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
        this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));
#else
    // Note: On Mac, the dock icon is used to provide the tray's functionality.
    MacDockIconHandler* dockIconHandler = MacDockIconHandler::instance();
    dockIconHandler->setMainWindow((QMainWindow*)this);
    trayIconMenu = dockIconHandler->dockMenu();
#endif
    // Configuration of the tray icon (or dock icon) icon menu
    trayIconMenu->addAction(toggleHideAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(sendCoinsAction);
    trayIconMenu->addAction(receiveCoinsAction);
    // trayIconMenu->addAction(privacyAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(signMessageAction);
    trayIconMenu->addAction(verifyMessageAction);
    trayIconMenu->addAction(bip38ToolAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(optionsAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(openInfoAction);
    trayIconMenu->addAction(openRPCConsoleAction);
    trayIconMenu->addAction(openNetworkAction);
    trayIconMenu->addAction(openPeersAction);
    trayIconMenu->addAction(openRepairAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(openConfEditorAction);
    trayIconMenu->addAction(openMNConfEditorAction);
    trayIconMenu->addAction(showBackupsAction);
    trayIconMenu->addAction(openBlockExplorerAction);
#ifndef Q_OS_MAC // This is built-in on Mac
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);
#endif
}
#ifndef Q_OS_MAC
void BitcoinGUI::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger) {
        // Click on system tray icon triggers show/hide of the main window
        toggleHidden();
    }
}
#endif

void BitcoinGUI::optionsClicked()
{
    if (!clientModel || !clientModel->getOptionsModel())
        return;
    OptionsDialog dlg(this, enableWallet);
    dlg.setModel(clientModel->getOptionsModel());
    dlg.setCurrentIndex(0);
    dlg.exec();
}

void BitcoinGUI::aboutClicked()
{
    if (!clientModel)
        return;
    HelpMessageDialog dlg(this, true);
    dlg.exec();
}

void BitcoinGUI::showHelpMessageClicked()
{
    HelpMessageDialog* help = new HelpMessageDialog(this, false);
    help->setAttribute(Qt::WA_DeleteOnClose);
    help->show();
}

#ifdef ENABLE_WALLET
void BitcoinGUI::openClicked()
{
    OpenURIDialog dlg(this);
    if (dlg.exec()) {
        emit receivedURI(dlg.getURI());
    }
}
void BitcoinGUI::gotoOverviewPage()
{
    overviewAction->setChecked(true);
    if (walletFrame) walletFrame->gotoOverviewPage();
}
void BitcoinGUI::gotoHistoryPage()
{
    historyAction->setChecked(true);
    if (walletFrame) walletFrame->gotoHistoryPage();
}
void BitcoinGUI::gotoMasternodePage()
{
    QSettings settings;
    if (settings.value("fShowMasternodesTab").toBool()) {
        masternodeAction->setChecked(true);
        if (walletFrame) walletFrame->gotoMasternodePage();
    }
}
void BitcoinGUI::gotoGovernancePage()
{
    // governanceAction->setChecked(true);
    // if (walletFrame) walletFrame->gotoGovernancePage();
}
void BitcoinGUI::gotoReceiveCoinsPage()
{
    receiveCoinsAction->setChecked(true);
    if (walletFrame) walletFrame->gotoReceiveCoinsPage();
}
// void BitcoinGUI::gotoPrivacyPage()
// {
//     privacyAction->setChecked(true);
//     if (walletFrame) walletFrame->gotoPrivacyPage();
// }

void BitcoinGUI::gotoSendCoinsPage(QString addr)
{
    sendCoinsAction->setChecked(true);
    if (walletFrame) walletFrame->gotoSendCoinsPage(addr);
}
void BitcoinGUI::gotoSignMessageTab(QString addr)
{
    if (walletFrame) walletFrame->gotoSignMessageTab(addr);
}
void BitcoinGUI::gotoVerifyMessageTab(QString addr)
{
    if (walletFrame) walletFrame->gotoVerifyMessageTab(addr);
}
void BitcoinGUI::gotoMultisigCreate()
{
    if(walletFrame) walletFrame->gotoMultisigDialog(0);
}
void BitcoinGUI::gotoMultisigSpend()
{
    if(walletFrame) walletFrame->gotoMultisigDialog(1);
}
void BitcoinGUI::gotoMultisigSign()
{
    if(walletFrame) walletFrame->gotoMultisigDialog(2);
}
void BitcoinGUI::gotoBip38Tool()
{
    if (walletFrame) walletFrame->gotoBip38Tool();
}
void BitcoinGUI::gotoMultiSendDialog()
{
    multiSendAction->setChecked(true);
    if (walletFrame)
        walletFrame->gotoMultiSendDialog();
}
void BitcoinGUI::gotoBlockExplorerPage()
{
    if (walletFrame) walletFrame->gotoBlockExplorerPage();
}
#endif // ENABLE_WALLET

void BitcoinGUI::setNumConnections(int count)
{
    QString icon;
    switch (count) {
    case 0:
        icon = QICON_PATH_CONNECTIONZERO;
        break;
    case 1:
        icon = QICON_PATH_CONNECTIONGTONE;
        break;
    case 2:
    case 3:
        icon = QICON_PATH_CONNECTIONGTTHREE;
        break;
    case 4:
    case 5:
    case 6:
        icon = QICON_PATH_CONNECTIONGTSIX;
        break;
    case 7:
    case 8:
    case 9:
        icon = QICON_PATH_CONNECTIONGTNINE;
        break;
    default:
        icon = QICON_PATH_CONNECTIONDEFAULT;
        break;
    }
    labelConnectionsIcon->setPixmap(QIcon(QString(icon)).pixmap(STATUSBAR_ICONSIZE, STATUSBAR_ICONSIZE));
    // QIcon connectionItem = QIcon(QString(icon)).pixmap(STATUSBAR_ICONSIZE, STATUSBAR_ICONSIZE);
    // labelConnectionsIcon->setPixmap(connectionItem);
    labelConnectionsIcon->setToolTip(tr("%n active connection(s) to NativeCoin network", "", count));
}

void BitcoinGUI::setNumBlocks(int count)
{
    if (!clientModel)
        return;

    // Prevent orphan statusbar messages (e.g. hover Quit in main menu, wait until chain-sync starts -> garbelled text)
    statusBar()->clearMessage();
    // Acquire current block source
    enum BlockSource blockSource = clientModel->getBlockSource();
    switch (blockSource) {
    case BLOCK_SOURCE_NETWORK:
        progressBarLabel->setText(tr("Synchronizing with network..."));
        break;
    case BLOCK_SOURCE_DISK:
        progressBarLabel->setText(tr("Importing blocks from disk..."));
        break;
    case BLOCK_SOURCE_REINDEX:
        progressBarLabel->setText(tr("Reindexing blocks on disk..."));
        break;
    case BLOCK_SOURCE_NONE:
        // Case: not Importing, not Reindexing and no network connection
        progressBarLabel->setText(tr("No block source available..."));
        break;
    }

    QString tooltip;
    QDateTime lastBlockDate = clientModel->getLastBlockDate();
    QDateTime currentDate   = QDateTime::currentDateTime();
    int secs                = lastBlockDate.secsTo(currentDate);
    tooltip                 = tr("Processed %n blocks of transaction history.", "", count);
    // Set icon state: spinning if catching up, tick otherwise
    //    if(secs < 25*60) // 90*60 for bitcoin but we are 4x times faster
    if (masternodeSync.IsBlockchainSynced()) {
        QString strSyncStatus;
        tooltip = tr("Up to date") + QString(".<br>") + tooltip;
        if (masternodeSync.IsSynced()) {
            progressBarLabel->setVisible(false);
            progressBar->setVisible(false);
            // Small label in bottom right corner
            labelBlocksIcon->setPixmap(QIcon(QICON_PATH_SYNCED).pixmap(STATUSBAR_ICONSIZE, STATUSBAR_ICONSIZE));
        } else {
            int nAttempt;
            int progress = 0;
            labelBlocksIcon->setPixmap(
                QIcon(
                    QString(
                        QICON_PATH_SPINNERGENERIC
                    ).arg(spinnerFrame, 3, 10, QChar('0'))).pixmap(STATUSBAR_ICONSIZE, STATUSBAR_ICONSIZE));
            spinnerFrame = (spinnerFrame + 1) % SPINNER_FRAMES;

#ifdef ENABLE_WALLET
            if (walletFrame)
                walletFrame->showOutOfSyncWarning(false);
#endif // ENABLE_WALLET
            nAttempt = masternodeSync.RequestedMasternodeAttempt < MASTERNODE_SYNC_THRESHOLD ?
                           masternodeSync.RequestedMasternodeAttempt + 1 :
                           MASTERNODE_SYNC_THRESHOLD;
            progress = nAttempt + (masternodeSync.RequestedMasternodeAssets - 1) * MASTERNODE_SYNC_THRESHOLD;
            progressBar->setMaximum(4 * MASTERNODE_SYNC_THRESHOLD);
            progressBar->setFormat(tr("Synchronizing additional data: %p%"));
            progressBar->setValue(progress);
        }

        strSyncStatus = QString(masternodeSync.GetSyncStatus().c_str());
        progressBarLabel->setText(strSyncStatus);
        tooltip = strSyncStatus + QString("<br>") + tooltip;
    } else {
        // Represent time from last generated block in human readable text
        QString timeBehindText;
        const int HOUR_IN_SECONDS = 60 * 60;
        const int DAY_IN_SECONDS = 24 * 60 * 60;
        const int WEEK_IN_SECONDS = 7 * 24 * 60 * 60;
        const int YEAR_IN_SECONDS = 31556952; // Average length of year in Gregorian calendar
        if (secs < 2 * DAY_IN_SECONDS) {
            timeBehindText = tr("%n hour(s)", "", secs / HOUR_IN_SECONDS);
        } else if (secs < 2 * WEEK_IN_SECONDS) {
            timeBehindText = tr("%n day(s)", "", secs / DAY_IN_SECONDS);
        } else if (secs < YEAR_IN_SECONDS) {
            timeBehindText = tr("%n week(s)", "", secs / WEEK_IN_SECONDS);
        } else {
            int years = secs / YEAR_IN_SECONDS;
            int remainder = secs % YEAR_IN_SECONDS;
            timeBehindText = tr("%1 and %2").arg(tr("%n year(s)", "", years)).arg(tr("%n week(s)", "", remainder / WEEK_IN_SECONDS));
        }

        progressBarLabel->setVisible(true);
        progressBar->setFormat(tr("%1 behind. Scanning block %2").arg(timeBehindText).arg(count));
        progressBar->setMaximum(1000000000);
        progressBar->setValue(clientModel->getVerificationProgress() * 1000000000.0 + 0.5);
        progressBar->setVisible(true);

        tooltip = tr("Catching up...") + QString("<br>") + tooltip;
        if (count != prevBlocks) {
            labelBlocksIcon->setPixmap(
                QIcon(
                    QString(
                        QICON_PATH_SPINNERGENERIC
                        ).arg(spinnerFrame, 3, 10, QChar('0'))).pixmap(STATUSBAR_ICONSIZE, STATUSBAR_ICONSIZE));
            spinnerFrame = (spinnerFrame + 1) % SPINNER_FRAMES;
        }
        prevBlocks = count;

#ifdef ENABLE_WALLET
        if (walletFrame)
            walletFrame->showOutOfSyncWarning(true);
#endif // ENABLE_WALLET

        tooltip += QString("<br>");
        tooltip += tr("Last received block was generated %1 ago.").arg(timeBehindText);
        tooltip += QString("<br>");
        tooltip += tr("Transactions after this will not yet be visible.");
    }

    // Don't word-wrap this (fixed-width) tooltip
    tooltip = QString("<nobr>") + tooltip + QString("</nobr>");

    labelBlocksIcon->setToolTip(tooltip);
    progressBarLabel->setToolTip(tooltip);
    progressBar->setToolTip(tooltip);
}

void BitcoinGUI::message(const QString& title, const QString& message, unsigned int style, bool* ret)
{
    QString strTitle = tr("NativeCoin Core"); // default title
    // Default to information icon
    int nMBoxIcon = QMessageBox::Information;
    int nNotifyIcon = Notificator::Information;

    QString msgType;

    // Prefer supplied title over style based title
    if (!title.isEmpty()) {
        msgType = title;
    } else {
        switch (style) {
        case CClientUIInterface::MSG_ERROR:
            msgType = tr("Error");
            break;
        case CClientUIInterface::MSG_WARNING:
            msgType = tr("Warning");
            break;
        case CClientUIInterface::MSG_INFORMATION:
            msgType = tr("Information");
            break;
        default:
            break;
        }
    }
    // Append title to "nativecoin - "
    if (!msgType.isEmpty())
        strTitle += " - " + msgType;

    // Check for error/warning icon
    if (style & CClientUIInterface::ICON_ERROR) {
        nMBoxIcon = QMessageBox::Critical;
        nNotifyIcon = Notificator::Critical;
    } else if (style & CClientUIInterface::ICON_WARNING) {
        nMBoxIcon = QMessageBox::Warning;
        nNotifyIcon = Notificator::Warning;
    }

    // Display message
    if (style & CClientUIInterface::MODAL) {
        // Check for buttons, use OK as default, if none was supplied
        QMessageBox::StandardButton buttons;
        if (!(buttons = (QMessageBox::StandardButton)(style & CClientUIInterface::BTN_MASK)))
            buttons = QMessageBox::Ok;

        showNormalIfMinimized();
        QMessageBox mBox((QMessageBox::Icon)nMBoxIcon, strTitle, message, buttons, this);
        int r = mBox.exec();
        if (ret != NULL)
            *ret = r == QMessageBox::Ok;
    } else
        notificator->notify((Notificator::Class)nNotifyIcon, strTitle, message);
}

void BitcoinGUI::changeEvent(QEvent* e)
{
    QMainWindow::changeEvent(e);
#ifndef Q_OS_MAC // Ignored on Mac
    if (e->type() == QEvent::WindowStateChange) {
        if (clientModel && clientModel->getOptionsModel() && clientModel->getOptionsModel()->getMinimizeToTray()) {
            QWindowStateChangeEvent* wsevt = static_cast<QWindowStateChangeEvent*>(e);
            if (!(wsevt->oldState() & Qt::WindowMinimized) && isMinimized()) {
                QTimer::singleShot(0, this, SLOT(hide()));
                e->ignore();
            }
        }
    }
#endif
}

void BitcoinGUI::closeEvent(QCloseEvent* event)
{
#ifndef Q_OS_MAC // Ignored on Mac
    if (clientModel && clientModel->getOptionsModel()) {
        if (!clientModel->getOptionsModel()->getMinimizeOnClose()) {
            QApplication::quit();
        }
    }
#endif
    QMainWindow::closeEvent(event);
}

#ifdef ENABLE_WALLET
void BitcoinGUI::incomingTransaction(const QString& date, int unit, const CAmount& amount, const QString& type, const QString& address)
{
    // Only send notifications when not disabled
    if(!bdisableSystemnotifications){
        // On new transaction, make an info balloon
        message((amount) < 0 ? (pwalletMain->fMultiSendNotify == true ? tr("Sent MultiSend transaction") : tr("Sent transaction")) : tr("Incoming transaction"),
            tr("Date: %1\n"
               "Amount: %2\n"
               "Type: %3\n"
               "Address: %4\n")
                .arg(date)
                .arg(BitcoinUnits::formatWithUnit(unit, amount, true))
                .arg(type)
                .arg(address),
            CClientUIInterface::MSG_INFORMATION);

        pwalletMain->fMultiSendNotify = false;
    }
}
#endif // ENABLE_WALLET

void BitcoinGUI::dragEnterEvent(QDragEnterEvent* event)
{
    // Accept only URIs
    if (event->mimeData()->hasUrls())
        event->acceptProposedAction();
}

void BitcoinGUI::dropEvent(QDropEvent* event)
{
    if (event->mimeData()->hasUrls()) {
        foreach (const QUrl& uri, event->mimeData()->urls()) {
            emit receivedURI(uri.toString());
        }
    }
    event->acceptProposedAction();
}

bool BitcoinGUI::eventFilter(QObject* object, QEvent* event)
{
    // Catch status tip events
    if (event->type() == QEvent::StatusTip) {
        // Prevent adding text from setStatusTip(), if we currently use the status bar for displaying other stuff
        if (progressBarLabel->isVisible() || progressBar->isVisible())
            return true;
    }
    return QMainWindow::eventFilter(object, event);
}

#ifdef ENABLE_WALLET
void BitcoinGUI::setStakingStatus()
{
    if (walletFrame) {
        if (pwalletMain)
            fMultiSend = pwalletMain->isMultiSendEnabled();

        if (nLastCoinStakeSearchInterval) {
            labelStakingIcon->show();
            labelStakingIcon->setPixmap(QIcon(QICON_PATH_STAKINGACTIVE).pixmap(STATUSBAR_ICONSIZE, STATUSBAR_ICONSIZE));
            labelStakingIcon->setToolTip(
                    tr("Staking is active\n MultiSend: %1").arg(fMultiSend ? tr("Active") : tr("Not Active")));
        } else {
            labelStakingIcon->show();
            labelStakingIcon->setPixmap(
                    QIcon(QICON_PATH_STAKINGINACTIVE).pixmap(STATUSBAR_ICONSIZE, STATUSBAR_ICONSIZE));
            labelStakingIcon->setToolTip(
                    tr("Staking is not active\n MultiSend: %1").arg(fMultiSend ? tr("Active") : tr("Not Active")));
        }
    }
}

void BitcoinGUI::setAutoMintStatus()
{
    // REMOVED
}

bool BitcoinGUI::handlePaymentRequest(const SendCoinsRecipient& recipient)
{
    // URI has to be valid
    if (walletFrame && walletFrame->handlePaymentRequest(recipient)) {
        showNormalIfMinimized();
        gotoSendCoinsPage();
        return true;
    }
    return false;
}

void BitcoinGUI::setEncryptionStatus(int status)
{
    switch (status) {
    case WalletModel::Unencrypted:
        labelEncryptionIcon->hide();
        encryptWalletAction->setChecked(false);
        changePassphraseAction->setEnabled(false);
        unlockWalletAction->setVisible(false);
        lockWalletAction->setVisible(false);
        encryptWalletAction->setEnabled(true);
        break;
    case WalletModel::Unlocked:
        labelEncryptionIcon->show();
        labelEncryptionIcon->setPixmap(QIcon(QString(QICON_PATH_UNENCRYPTED)).pixmap(STATUSBAR_ICONSIZE, STATUSBAR_ICONSIZE));
        labelEncryptionIcon->setToolTip(tr("Wallet is <b>encrypted</b> and currently <b>unlocked</b>"));
        encryptWalletAction->setChecked(true);
        changePassphraseAction->setEnabled(true);
        unlockWalletAction->setVisible(false);
        lockWalletAction->setVisible(true);
        encryptWalletAction->setEnabled(false); // TODO: decrypt currently not supported
        break;
    case WalletModel::UnlockedForAnonymizationOnly:
        labelEncryptionIcon->show();
        labelEncryptionIcon->setPixmap(QIcon(QString(QICON_PATH_UNENCRYPTED)).pixmap(STATUSBAR_ICONSIZE, STATUSBAR_ICONSIZE));
        labelEncryptionIcon->setToolTip(tr("Wallet is <b>encrypted</b> and currently <b>unlocked</b> for anonymization and staking only"));
        encryptWalletAction->setChecked(true);
        changePassphraseAction->setEnabled(true);
        unlockWalletAction->setVisible(true);
        lockWalletAction->setVisible(true);
        encryptWalletAction->setEnabled(false); // TODO: decrypt currently not supported
        break;
    case WalletModel::Locked:
        labelEncryptionIcon->show();
        labelEncryptionIcon->setPixmap(QIcon(QString(QICON_PATH_ENCRYPTED)).pixmap(STATUSBAR_ICONSIZE, STATUSBAR_ICONSIZE));
        labelEncryptionIcon->setToolTip(tr("Wallet is <b>encrypted</b> and currently <b>locked</b>"));
        encryptWalletAction->setChecked(true);
        changePassphraseAction->setEnabled(true);
        unlockWalletAction->setVisible(true);
        lockWalletAction->setVisible(false);
        encryptWalletAction->setEnabled(false); // TODO: decrypt currently not supported
        break;
    }
}
#endif // ENABLE_WALLET

void BitcoinGUI::updateTorIcon()
{
    std::string ip_port;
    bool tor_enabled = clientModel->getTorInfo(ip_port);

    if (tor_enabled) {
        if (labelTorIcon->pixmap() == 0) {
            QString ip_port_q = QString::fromStdString(ip_port);
            labelTorIcon->setPixmap(QIcon(QICON_PATH_TOR).pixmap(STATUSBAR_ICONSIZE, STATUSBAR_ICONSIZE));
            labelTorIcon->setToolTip(tr("Tor is <b>enabled</b>: %1").arg(ip_port_q));
        } else {
            labelTorIcon->show();
        }
    } else {
        labelTorIcon->hide();
    }
}

void BitcoinGUI::showNormalIfMinimized(bool fToggleHidden)
{
    if (!clientModel)
        return;
    // activateWindow() (sometimes) helps with keyboard focus on Windows
    if (isHidden()) {
        show();
        activateWindow();
    } else if (isMinimized()) {
        showNormal();
        activateWindow();
    } else if (GUIUtil::isObscured(this)) {
        raise();
        activateWindow();
    } else if (fToggleHidden)
        hide();
}

void BitcoinGUI::toggleHidden()
{
    showNormalIfMinimized(true);
}

void BitcoinGUI::detectShutdown()
{
    if (ShutdownRequested()) {
        if (rpcConsole)
            rpcConsole->hide();
        qApp->quit();
    }
}

void BitcoinGUI::showProgress(const QString& title, int nProgress)
{
    if (nProgress == 0) {
        progressDialog = new QProgressDialog(title, "", 0, 100);
        progressDialog->setWindowModality(Qt::ApplicationModal);
        progressDialog->setMinimumDuration(0);
        progressDialog->setCancelButton(0);
        progressDialog->setAutoClose(false);
        progressDialog->setValue(0);
    } else if (nProgress == 100) {
        if (progressDialog) {
            progressDialog->close();
            progressDialog->deleteLater();
        }
    } else if (progressDialog)
        progressDialog->setValue(nProgress);
}

static bool ThreadSafeMessageBox(BitcoinGUI* gui, const std::string& message, const std::string& caption, unsigned int style)
{
    bool modal = (style & CClientUIInterface::MODAL);
    // The SECURE flag has no effect in the Qt GUI.
    // bool secure = (style & CClientUIInterface::SECURE);
    style &= ~CClientUIInterface::SECURE;
    bool ret = false;
    // In case of modal message, use blocking connection to wait for user to click a button
    QMetaObject::invokeMethod(gui, "message",
        modal ? GUIUtil::blockingGUIThreadConnection() : Qt::QueuedConnection,
        Q_ARG(QString, QString::fromStdString(caption)),
        Q_ARG(QString, QString::fromStdString(message)),
        Q_ARG(unsigned int, style),
        Q_ARG(bool*, &ret));
    return ret;
}

void BitcoinGUI::subscribeToCoreSignals()
{
    // Connect signals to client
    uiInterface.ThreadSafeMessageBox.connect(boost::bind(ThreadSafeMessageBox, this, _1, _2, _3));
}

void BitcoinGUI::unsubscribeFromCoreSignals()
{
    // Disconnect signals from client
    uiInterface.ThreadSafeMessageBox.disconnect(boost::bind(ThreadSafeMessageBox, this, _1, _2, _3));
}

/** Get restart command-line parameters and request restart */
void BitcoinGUI::handleRestart(QStringList args)
{
    if (!ShutdownRequested())
        emit requestedRestart(args);
}

UnitDisplayStatusBarControl::UnitDisplayStatusBarControl() : optionsModel(0),
                                                             menu(0)
{
    createContextMenu();
    setToolTip(tr("Unit to show amounts in. Click to select another unit."));
}

/** So that it responds to button clicks */
void UnitDisplayStatusBarControl::mousePressEvent(QMouseEvent* event)
{
    onDisplayUnitsClicked(event->pos());
}

/** Creates context menu, its actions, and wires up all the relevant signals for mouse events. */
void UnitDisplayStatusBarControl::createContextMenu()
{
    menu = new QMenu();
    foreach (BitcoinUnits::Unit u, BitcoinUnits::availableUnits()) {
        QAction* menuAction = new QAction(QString(BitcoinUnits::name(u)), this);
        menuAction->setData(QVariant(u));
        menu->addAction(menuAction);
    }
    connect(menu, SIGNAL(triggered(QAction*)), this, SLOT(onMenuSelection(QAction*)));
}

/** Lets the control know about the Options Model (and its signals) */
void UnitDisplayStatusBarControl::setOptionsModel(OptionsModel* optionsModel)
{
    if (optionsModel) {
        this->optionsModel = optionsModel;

        // be aware of a display unit change reported by the OptionsModel object.
        connect(optionsModel, SIGNAL(displayUnitChanged(int)), this, SLOT(updateDisplayUnit(int)));

        // initialize the display units label with the current value in the model.
        updateDisplayUnit(optionsModel->getDisplayUnit());
    }
}

/** When Display Units are changed on OptionsModel it will refresh the display text of the control on the status bar */
void UnitDisplayStatusBarControl::updateDisplayUnit(int newUnits)
{
    if (Params().NetworkID() == CBaseChainParams::MAIN) {
        setPixmap(QIcon(QICON_PATH_UNITGENERIC + BitcoinUnits::id(newUnits)).pixmap(39, STATUSBAR_ICONSIZE));
    } else {
        setPixmap(QIcon(QICON_PATH_UNITGENERICT + BitcoinUnits::id(newUnits)).pixmap(39, STATUSBAR_ICONSIZE));
    }
}

/** Shows context menu with Display Unit options by the mouse coordinates */
void UnitDisplayStatusBarControl::onDisplayUnitsClicked(const QPoint& point)
{
    QPoint globalPos = mapToGlobal(point);
    menu->exec(globalPos);
}

/** Tells underlying optionsModel to update its current display unit. */
void UnitDisplayStatusBarControl::onMenuSelection(QAction* action)
{
    if (action) {
        optionsModel->setDisplayUnit(action->data());
    }
}
