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
// Main
//==============================================================================

#include "widget.h"

//==============================================================================

#include <QFileInfo>
#include <QLoggingCategory>

//==============================================================================

#include <QtSingleApplication>

//==============================================================================

#ifdef Q_OS_WIN
    #include <Windows.h>
#endif

//==============================================================================

int main(int pArgC, char *pArgV[])
{
    // Create our application, after making sure that on Windows we can handle
    // scaled HiDPI screens

#ifdef Q_OS_WIN
    SetProcessDPIAware();
#endif

    QtSingleApplication app(QFileInfo(pArgV[0]).baseName(), pArgC, pArgV);

    // Check whether another instance of our application is already running and
    // leave if that's the case

    if (app.isRunning()) {
        return 0;
    }

    // Filter out OpenSSL warning messages

    QLoggingCategory::setFilterRules("qt.network.ssl.warning=false");

    // Customise our application

    app.setApplicationName("WaniKani");
    app.setOrganizationName("Hellix");

    // Create our widget

    new Widget();

    // Execute our application

    return app.exec();
}

//==============================================================================
// End of file
//==============================================================================
