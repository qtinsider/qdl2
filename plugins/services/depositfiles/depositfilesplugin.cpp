/**
 * Copyright (C) 2016 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 3, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "depositfilesplugin.h"
#include "captchatype.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QScriptEngine>
#include <QTimer>
#if QT_VERSION >= 0x050000
#include <QUrlQuery>
#else
#include <QtPlugin>
#endif
#ifdef DEPOSITFILES_DEBUG
#include <QDebug>
#endif

const QRegExp DepositFilesPlugin::FILE_REGEXP("(http(s|):|)//fileshare\\d+\\.(depositfiles|dfiles)\\.\\w+/[^'\"]+");
const QString DepositFilesPlugin::LOGIN_URL("https://depositfiles.com/api/user/login");
const QString DepositFilesPlugin::REQUEST_URL("https://depositfiles.com/get_file.php");
const QString DepositFilesPlugin::RECAPTCHA_PLUGIN_ID("qdl2-solvemediarecaptcha");

const int DepositFilesPlugin::MAX_REDIRECTS = 8;

DepositFilesPlugin::DepositFilesPlugin(QObject *parent) :
    ServicePlugin(parent),
    m_nam(0),
    m_waitTimer(0),
    m_redirects(0),
    m_ownManager(false)
{
}

QString DepositFilesPlugin::getRedirect(const QNetworkReply *reply) {
    QString redirect = QString::fromUtf8(reply->rawHeader("Location"));
    
    if (redirect.startsWith("//")) {
        redirect.prepend(reply->url().scheme() + ":");
    }
    else if (redirect.startsWith("/")) {
        redirect.prepend(reply->url().scheme() + "://" + reply->url().authority());
    }
    
    return redirect;
}

QNetworkAccessManager* DepositFilesPlugin::networkAccessManager() {
    if (!m_nam) {
        m_nam = new QNetworkAccessManager(this);
        m_ownManager = true;
    }

    return m_nam;
}

void DepositFilesPlugin::setNetworkAccessManager(QNetworkAccessManager *manager) {
    if (!manager) {
        return;
    }
    
    if ((m_ownManager) && (m_nam)) {
        delete m_nam;
        m_nam = 0;
    }

    m_nam = manager;
    m_ownManager = false;
}

bool DepositFilesPlugin::cancelCurrentOperation() {
    stopWaitTimer();
    m_redirects = 0;
    emit currentOperationCanceled();
    return true;
}

void DepositFilesPlugin::checkUrl(const QString &url, const QVariantMap &) {
#ifdef DEPOSITFILES_DEBUG
    qDebug() << "DepositFilesPlugin::checkUrl(). URL:" << url;
#endif
    m_redirects = 0;
    QNetworkRequest request(QUrl::fromUserInput(url));
    request.setRawHeader("Accept-Language", "en-GB,en-US;q=0.8,en;q=0.6");
    QNetworkReply *reply = networkAccessManager()->get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(checkUrlIsValid()));
    connect(this, SIGNAL(currentOperationCanceled()), reply, SLOT(deleteLater()));
}

void DepositFilesPlugin::getDownloadRequest(const QString &url, const QVariantMap &settings) {
#ifdef DEPOSITFILES_DEBUG
    qDebug() << "DepositFilesPlugin::getDownloadRequest(). URL:" << url;
#endif
    m_redirects = 0;
    m_url = QUrl::fromUserInput(url);

    if (settings.value("Account/useLogin", false).toBool()) {
        const QString username = settings.value("Account/username").toString();
        const QString password = settings.value("Account/password").toString();

        if ((username.isEmpty()) || (password.isEmpty())) {
            QVariantList list;
            QVariantMap usernameMap;
            usernameMap["type"] = "text";
            usernameMap["label"] = tr("Username");
            usernameMap["key"] = "username";
            list << usernameMap;
            QVariantMap passwordMap;
            passwordMap["type"] = "password";
            passwordMap["label"] = tr("Password");
            passwordMap["key"] = "password";
            list << passwordMap;
            emit settingsRequest(tr("Login"), list, "submitLogin");
        }   
        else {
            login(username, password);
        }

        return;
    }
    
    fetchDownloadRequest(m_url);
}

void DepositFilesPlugin::submitCaptchaResponse(const QString &challenge, const QString &response) {
    m_redirects = 0;
    QUrl url(REQUEST_URL);
    url.setHost(m_url.host());
#if QT_VERSION >= 0x050000
    QUrlQuery query(url);
    query.addQueryItem("fid", m_fileId);
    query.addQueryItem("challenge", challenge);
    query.addQueryItem("response", response);
    query.addQueryItem("acpuzzle", "1");
    url.setQuery(query);
#else
    url.addQueryItem("fid", m_fileId);
    url.addQueryItem("challenge", challenge);
    url.addQueryItem("response", response);
    url.addQueryItem("acpuzzle", "1");
#endif
#ifdef DEPOSITFILES_DEBUG
    qDebug() << "DepositFilesPlugin::submitCaptchaResponse(). URL:" << url;
#endif
    QNetworkRequest request(url);
    request.setRawHeader("Accept-Language", "en-GB,en-US;q=0.8,en;q=0.6");
    request.setRawHeader("X-Requested-With", "XMLHttpRequest");
    QNetworkReply *reply = networkAccessManager()->get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(checkCaptcha()));
    connect(this, SIGNAL(currentOperationCanceled()), reply, SLOT(deleteLater()));
}

void DepositFilesPlugin::submitLogin(const QVariantMap &credentials) {
    if ((credentials.contains("username")) && (credentials.contains("password"))) {
        const QString username = credentials.value("username").toString();
        const QString password = credentials.value("password").toString();

        if ((!username.isEmpty()) && (!password.isEmpty())) {
            login(username, password);
            return;
        }
    }

    emit error(tr("Invalid login credentials provided"));
}

void DepositFilesPlugin::fetchDownloadRequest(const QUrl &url) {
#ifdef DEPOSITFILES_DEBUG
    qDebug() << "DepositFilesPlugin::fetchDownloadRequest(). URL:" << url;
#endif
    m_redirects = 0;
    QNetworkRequest request(url);
    request.setRawHeader("Accept-Language", "en-GB,en-US;q=0.8,en;q=0.6");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QNetworkReply *reply = networkAccessManager()->post(request, "gateway_result=1");
    connect(reply, SIGNAL(finished()), this, SLOT(checkDownloadRequest()));
    connect(this, SIGNAL(currentOperationCanceled()), reply, SLOT(deleteLater()));
}

void DepositFilesPlugin::followRedirect(const QUrl &url, const char* slot) {
#ifdef DEPOSITFILES_DEBUG
    qDebug() << "DepositFilesPlugin::followRedirect(). URL:" << url;
#endif
    m_redirects++;
    QNetworkRequest request(url);
    request.setRawHeader("Accept-Language", "en-GB,en-US;q=0.8,en;q=0.6");
    QNetworkReply *reply = networkAccessManager()->get(request);
    connect(reply, SIGNAL(finished()), this, slot);
    connect(this, SIGNAL(currentOperationCanceled()), reply, SLOT(deleteLater()));
}

void DepositFilesPlugin::getCaptchaKey() {
    m_redirects = 0;
    QUrl url(REQUEST_URL);
    url.setHost(m_url.host());
#if QT_VERSION >= 0x050000
    QUrlQuery query(url);
    query.addQueryItem("fid", m_fileId);
    url.setQuery(query);
#else
    url.addQueryItem("fid", m_fileId);
#endif
    QNetworkRequest request(url);
#ifdef DEPOSITFILES_DEBUG
    qDebug() << "DepositFilesPlugin::getCaptchaKey(). URL:" << url;
#endif
    request.setRawHeader("Accept-Language", "en-GB,en-US;q=0.8,en;q=0.6");
    request.setRawHeader("X-Requested-With", "XMLHttpRequest");
    QNetworkReply *reply = networkAccessManager()->get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(checkCaptchaKey()));
    connect(this, SIGNAL(currentOperationCanceled()), reply, SLOT(deleteLater()));
}

void DepositFilesPlugin::login(const QString &username, const QString &password) {
    m_redirects = 0;
    const QString data = QString("login=%1&password=%2").arg(username).arg(password);
    QNetworkRequest request(LOGIN_URL);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QNetworkReply *reply = networkAccessManager()->post(request, data.toUtf8());
    connect(reply, SIGNAL(finished()), this, SLOT(checkLogin()));
    connect(this, SIGNAL(currentOperationCanceled()), reply, SLOT(deleteLater()));
}

void DepositFilesPlugin::startWaitTimer(int msecs, const char* slot) {
    if (!m_waitTimer) {
        m_waitTimer = new QTimer(this);
        m_waitTimer->setSingleShot(true);
    }

    m_waitTimer->setInterval(msecs);
    m_waitTimer->start();
    emit waitRequest(msecs, false);
    disconnect(m_waitTimer, SIGNAL(timeout()), this, 0);
    connect(m_waitTimer, SIGNAL(timeout()), this, slot);
}

void DepositFilesPlugin::stopWaitTimer() {
    if (m_waitTimer) {
        m_waitTimer->stop();
        disconnect(m_waitTimer, SIGNAL(timeout()), this, 0);
    }
}

void DepositFilesPlugin::checkUrlIsValid() {
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());

    if (!reply) {
        emit error(tr("Network error"));
        return;
    }

    const QString redirect = getRedirect(reply);
    
    if (!redirect.isEmpty()) {
        if (FILE_REGEXP.indexIn(redirect) == 0) {
            emit urlChecked(UrlResult(reply->request().url().toString(),
                            redirect.mid(redirect.lastIndexOf("/") + 1)));
        }
        else if (m_redirects < MAX_REDIRECTS) {
            followRedirect(redirect, SLOT(checkUrlIsValid()));
        }
        else {
            emit error(tr("Maximum redirects reached"));
        }

        reply->deleteLater();
        return;
    }

    switch (reply->error()) {
    case QNetworkReply::NoError:
        break;
    case QNetworkReply::OperationCanceledError:
        reply->deleteLater();
        return;
    default:
        emit error(reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString());
        reply->deleteLater();
        return;
    }

    const QString response = QString::fromUtf8(reply->readAll());

    if (response.contains("file does not exist")) {
        emit error(tr("File not found"));
        return;
    }

    const QString script = response.section("eval( ", 1, 1).section(");", 0, 0);

    if (!script.isEmpty()) {
        QScriptEngine engine;
        const QScriptValue result = engine.evaluate(script).toString();

        if (result.isString()) {
            const QString fileName = result.toString().section("title=\"", 1, 1).section('"', 0, 0);

            if (!fileName.isEmpty()) {
                emit urlChecked(UrlResult(reply->request().url().toString(), fileName));
                reply->deleteLater();
                return;
            }
        }
    }

    emit error(tr("Unknown error"));
    reply->deleteLater();
}

void DepositFilesPlugin::checkDownloadRequest() {
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());

    if (!reply) {
        emit error(tr("Network error"));
        return;
    }

    const QString redirect = getRedirect(reply);
    
    if (!redirect.isEmpty()) {
        if (FILE_REGEXP.indexIn(redirect) == 0) {
            emit downloadRequest(QNetworkRequest(redirect));
        }
        else if (m_redirects < MAX_REDIRECTS) {
            followRedirect(redirect, SLOT(checkDownloadRequest()));
        }
        else {
            emit error(tr("Maximum redirects reached"));
        }

        reply->deleteLater();
        return;
    }

    switch (reply->error()) {
    case QNetworkReply::NoError:
        break;
    case QNetworkReply::OperationCanceledError:
        reply->deleteLater();
        return;
    default:
        emit error(reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString());
        reply->deleteLater();
        return;
    }

    const QString response = QString::fromUtf8(reply->readAll());
    reply->deleteLater();

    if (FILE_REGEXP.indexIn(response) != -1) {
        QUrl url(FILE_REGEXP.cap());

        if (url.scheme().isEmpty()) {
            url.setScheme(m_url.scheme());
        }

        emit downloadRequest(QNetworkRequest(url));
        return;
    }

    if (response.contains("file does not exist")) {
        emit error(tr("File not found"));
        return;
    }

    if (response.contains("html_download_api-limit_interval")) {
        const int secs = response.section("html_download_api-limit_interval\">", 1, 1).section("<", 0, 0).toInt();
        
        if (secs > 0) {
            emit waitRequest(secs * 1000, true);
        }
        else {
            emit error(tr("Unknown error"));
        }

        return;
    }

    m_fileId = response.section("var fid = '", 1, 1).section("'", 0, 0);

    if (m_fileId.isEmpty()) {
        emit error(tr("Unknown error"));
    }
    else {
        startWaitTimer(60000, SLOT(getCaptchaKey()));
    }
}

void DepositFilesPlugin::checkCaptchaKey() {
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());

    if (!reply) {
        emit error(tr("Network error"));
        return;
    }

    const QString redirect = getRedirect(reply);
    
    if (!redirect.isEmpty()) {
        if (FILE_REGEXP.indexIn(redirect) == 0) {
            emit downloadRequest(QNetworkRequest(redirect));
        }
        else if (m_redirects < MAX_REDIRECTS) {
            followRedirect(redirect, SLOT(checkCaptchaKey()));
        }
        else {
            emit error(tr("Maximum redirects reached"));
        }

        reply->deleteLater();
        return;
    }

    switch (reply->error()) {
    case QNetworkReply::NoError:
        break;
    case QNetworkReply::OperationCanceledError:
        reply->deleteLater();
        return;
    default:
        emit error(reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString());
        reply->deleteLater();
        return;
    }
    
    const QString response = QString::fromUtf8(reply->readAll());
    m_recaptchaKey = response.section("ACPuzzleKey = '", 1, 1).section('\'', 0, 0);
    
    if (m_recaptchaKey.isEmpty()) {
        emit error(tr("No captcha key found"));
    }
    else {
        emit captchaRequest(RECAPTCHA_PLUGIN_ID, CaptchaType::Image, m_recaptchaKey, "submitCaptchaResponse");
    }
    
    reply->deleteLater();
}

void DepositFilesPlugin::checkLogin() {
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());

    if (!reply) {
        fetchDownloadRequest(m_url);
        return;
    }

    fetchDownloadRequest(m_url);
    reply->deleteLater();
}

void DepositFilesPlugin::checkCaptcha() {
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());

    if (!reply) {
        emit error(tr("Network error"));
        return;
    }

    const QString redirect = getRedirect(reply);
    
    if (!redirect.isEmpty()) {
        if (FILE_REGEXP.indexIn(redirect) == 0) {
            emit downloadRequest(QNetworkRequest(redirect));
        }
        else if (m_redirects < MAX_REDIRECTS) {
            followRedirect(redirect, SLOT(checkCaptcha()));
        }
        else {
            emit error(tr("Maximum redirects reached"));
        }

        reply->deleteLater();
        return;
    }

    switch (reply->error()) {
    case QNetworkReply::NoError:
        break;
    case QNetworkReply::OperationCanceledError:
        reply->deleteLater();
        return;
    default:
        emit error(reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString());
        reply->deleteLater();
        return;
    }

    const QString response = QString::fromUtf8(reply->readAll());

    if (FILE_REGEXP.indexIn(response) != -1) {
        QUrl url(FILE_REGEXP.cap());

        if (url.scheme().isEmpty()) {
            url.setScheme(m_url.scheme());
        }

        emit downloadRequest(QNetworkRequest(url));
    }
    else if (response.contains("check_recaptcha")) {
        emit captchaRequest(RECAPTCHA_PLUGIN_ID, CaptchaType::Image, m_recaptchaKey, "submitCaptchaResponse");
    }
    else {
        emit error(tr("Unknown error"));
    }

    reply->deleteLater();
}

ServicePlugin* DepositFilesPluginFactory::createPlugin(QObject *parent) {
    return new DepositFilesPlugin(parent);
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(qdl2-depositfiles, DepositFilesPluginFactory)
#endif
