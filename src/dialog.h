/*******************************************************************************

Copyright Alan Garny

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

*******************************************************************************/

//==============================================================================
// Main window
//==============================================================================

#pragma once

//==============================================================================

#include <QDialog>
#include <QSystemTrayIcon>

//==============================================================================

namespace Ui {
    class Dialog;
}

//==============================================================================

class QAbstractButton;

//==============================================================================

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog();
    ~Dialog();

protected:
    virtual void closeEvent(QCloseEvent *pEvent);

private:
    Ui::Dialog *mGui;

    QSystemTrayIcon *mTrayIcon;
    QMenu *mTrayIconMenu;

    QAction *mWaniKaniAction;
    QAction *mQuitAction;

    QTimer *mTimer;

    QString mApiKey;

private slots:
    void trayIconActivated(const QSystemTrayIcon::ActivationReason &pReason);

    void showDialog();

    void on_buttonBox_clicked(QAbstractButton *pButton);

    void updateWallpaper();
    void setWallpaper(const QString &pWallpaperFileName);
};

//==============================================================================
// End of file
//==============================================================================
