/*
 * Copyright (C) 2016 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

var request = null;

function checkUrl(url) {
    request = new XMLHttpRequest();
    request.onreadystatechange = function () {
        if (request.readyState == 4) {
            if (url.indexOf("/g/") != -1) {
                // Gallery
                try {
                    var gallery = request.responseText.split("gallery-view-content")[1].split("<div")[0];
                    var ids = gallery.match(/\w+(?=\"><img)/g);

                    if ((ids) && (ids.length > 0)) {
                        var results = [];
                        
                        for (var i = 0; i < ids.length; i++) {
                            results.push({"url": "http://imgbox.com/" + ids[i], "fileName": ids[i]});
                        }

                        try {
                            var packageName = request.responseText.split("id=\"gallery-view\"")[1]
                                .split("<div")[0].split("<h1>")[1].split("</h1>")[0];
                            urlChecked(results, packageName);
                        }
                        catch(err) {
                            urlChecked(results, url.substring(url.lastIndexOf("/") + 1));
                        }
                    }
                    else {
                        error(qsTr("File not found"));
                    }
                }
                catch(err) {
                    error(err);
                }
            }
            else {
                // Image
                try {
                    if (settings.value("retrieveGallery" , false)) {
                        // Try to retrieve the gallery
                        var galleryLink = /\/g\/\w+/.exec(request.responseText);
                        
                        if (galleryLink) {
                            checkUrl("http://imgbox.com" + galleryLink);
                            return;
                        }
                    }
                        
                    var container = request.responseText.split("class=\"image-content\"")[1].split("/>")[0];
                    var link = container.split("src=\"")[1].split("\"")[0];

                    if (link) {
                        var fileName = container.split("title=\"")[1].split("\"")[0];

                        if (!fileName) {
                            fileName = link.substring(link.lastIndexOf("/") + 1);
                        }
                        
                        urlChecked({"url": url, "fileName": fileName});
                    }
                    else {
                        error(qsTr("File not found"));
                    }
                }
                catch(err) {
                    error(err);
                }
            }
        }
    }

    request.open("GET", url);
    request.send();
}

function getDownloadRequest(url) {
    request = new XMLHttpRequest();
    request.onreadystatechange = function () {
        if (request.readyState == 4) {
            try {
                var container = request.responseText.split("class=\"image-content\"")[1].split("/>")[0];
                var link = container.split("src=\"")[1].split("\"")[0];

                if (link) {
                    downloadRequest({"url": link});
                }
                else {
                    error(qsTr("File not found"));
                }
            }
            catch(err) {
                error(err);
            }
        }
    }

    request.open("GET", url);
    request.send();
}

function cancelCurrentOperation() {
    if (request) {
        request.abort();
    }

    return true;
}
