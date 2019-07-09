// Copyright (c) 2017-2019 The nativecoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef zNATIVECONTROLDIALOG_H
#define zNATIVECONTROLDIALOG_H

#include <QDialog>
#include <QTreeWidgetItem>
#include "zNATIVE/zerocoin.h"
#include "privacydialog.h"

class CZerocoinMint;
class WalletModel;

namespace Ui {
class zNATIVEControlDialog;
}

class CzNATIVEControlWidgetItem : public QTreeWidgetItem
{
public:
    explicit CzNATIVEControlWidgetItem(QTreeWidget *parent, int type = Type) : QTreeWidgetItem(parent, type) {}
    explicit CzNATIVEControlWidgetItem(int type = Type) : QTreeWidgetItem(type) {}
    explicit CzNATIVEControlWidgetItem(QTreeWidgetItem *parent, int type = Type) : QTreeWidgetItem(parent, type) {}

    bool operator<(const QTreeWidgetItem &other) const;
};

class zNATIVEControlDialog : public QDialog
{
    Q_OBJECT

public:
    explicit zNATIVEControlDialog(QWidget *parent);
    ~zNATIVEControlDialog();

    void setModel(WalletModel* model);

    static std::set<std::string> setSelectedMints;
    static std::set<CMintMeta> setMints;
    static std::vector<CMintMeta> GetSelectedMints();

private:
    Ui::zNATIVEControlDialog *ui;
    WalletModel* model;
    PrivacyDialog* privacyDialog;

    void updateList();
    void updateLabels();

    enum {
        COLUMN_CHECKBOX,
        COLUMN_DENOMINATION,
        COLUMN_PUBCOIN,
        COLUMN_VERSION,
        COLUMN_PRECOMPUTE,
        COLUMN_CONFIRMATIONS,
        COLUMN_ISSPENDABLE
    };
    friend class CzNATIVEControlWidgetItem;

private slots:
    void updateSelection(QTreeWidgetItem* item, int column);
    void ButtonAllClicked();
};

#endif // zNATIVECONTROLDIALOG_H
