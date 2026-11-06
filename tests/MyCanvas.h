/*
 * Copyright (c) 2026 TAMBO ai and others
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 */

#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include "modelsTM.hpp"
#include <future>


using namespace ai::tambo::tmodeler;

class MyCanvas : public wxPanel {
    std::future<void> asyncSelector;
    std::future<void> asyncSelector2;
    std::mutex geoMutex;

private:
    Spot spotFromMouse(const wxMouseEvent& evt) {
        double logicalX = centerNetwork.x + (evt.GetX() - GetSize().GetWidth() / 2.0) / scale;
        double logicalY = centerNetwork.y - (evt.GetY() - GetSize().GetHeight() / 2.0) / scale;
        return Spot(logicalX, logicalY);
    }


public:
    std::vector<TGeo> geometries;
    std::vector<TGeo> base_geometries;

private:
    double scale = 100000.0;
    double offsetX = 0.0;
    double offsetY = 0.0;
    Spot centerNetwork;
    wxPoint mousePos;
    Spot start, end, current;
    int icurrent;

public:
    MyCanvas(wxWindow* parent) : wxPanel(parent) {
        SetBackgroundStyle(wxBG_STYLE_PAINT);
        Bind(wxEVT_PAINT, &MyCanvas::OnPaint, this);

        Bind(wxEVT_MOTION, [this](wxMouseEvent& evt) {
            current = spotFromMouse(evt);
            detection(current);
        });

        Bind(wxEVT_LEFT_DOWN, [this](wxMouseEvent& evt) {
            wxPoint pt = evt.GetPosition();
            start = spotFromMouse(evt);
            ///
            TGeo geo = spotFromMouse(evt);
            geo.label = " ";
            geo.style.borderColor = 0x22aa22;
            geo.style.borderWidth = 2;
            geo.style.selected = true;
            geometries.push_back(geo);
            Refresh();
        });

        Bind(wxEVT_RIGHT_DOWN, [this](wxMouseEvent& evt) {
            wxPoint pt = evt.GetPosition();
            end = spotFromMouse(evt);
            direction(start, end);
            ///
            TGeo geo = spotFromMouse(evt);
            geo.label = " ";
            geo.style.borderColor = 0x22aa22;
            geo.style.borderWidth = 5;
            geo.style.selected = true;
            geometries.push_back(geo);
            Refresh();
        });

        Bind(wxEVT_MOUSEWHEEL, [this](wxMouseEvent& evt) {
            int rot = evt.GetWheelRotation();
            double zoomFactor = 1.1;
            scale *= (rot > 0) ? zoomFactor : (1.0 / zoomFactor);
            Refresh();
        });


        wxSize sz = GetSize();
        offsetX = sz.GetWidth() / 2.0;
        offsetY = sz.GetHeight() / 2.0;

        centerNetwork = Spot(9.7036859, 4.0453016);
    }

private:
    void detection(const Spot& current) {
        if (asyncSelector2.valid() &&
            asyncSelector2.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready)
            return;

        asyncSelector2 = std::async(std::launch::async, [=]() {
            std::vector<TGeo> updated;

            {
                std::lock_guard<std::mutex> lock(geoMutex);
            }

            try
            {
                auto road = Direction1::nearestRoad(current);
                auto data = Direction1::parse(road);
                if (icurrent == -1) {
                    icurrent = geometries.size();
                    geometries.push_back(data);
                }
                else {
                    if (icurrent < geometries.size()) {
                        geometries[icurrent] = data;
                    }
                    else {
                        icurrent = geometries.size();
                        geometries.push_back(data);
                    }
                }

                // Remplacer les données sur le thread UI
                CallAfter([=]() {
                    std::lock_guard<std::mutex> lock(geoMutex);
                    Refresh();
                    });
            }
            catch (...) {}
        });
    }

    void direction(const Spot& start, const Spot& end) {
        if (asyncSelector.valid() &&
            asyncSelector.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready)
            return;

        asyncSelector = std::async(std::launch::async, [=]() {
            std::vector<TGeo> updated;

            {
                std::lock_guard<std::mutex> lock(geoMutex);
                updated = base_geometries;
            }

            try
            {
                std::vector<TGeo> roads = Direction1::direction(start, end);
                updated.insert(updated.end(), roads.begin(), roads.end());

                // Remplacer les données sur le thread UI
                CallAfter([=]() {
                    std::lock_guard<std::mutex> lock(geoMutex);
                    geometries = updated;
                    icurrent = -1;
                    Refresh();
                    });
            }
            catch (...) {}
        });
    }

private:
    wxColour intToColor(int rgb, int alpha = 255) {
        return wxColour((rgb >> 16) & 0xFF, (rgb >> 8) & 0xFF, rgb & 0xFF, alpha);
    }

    void drawLabel(wxDC& dc, const wxString& label, double x, double y) {
        if (label.IsEmpty()) return;
        dc.SetTextForeground(*wxBLACK);
        dc.SetFont(wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
        wxCoord w, h;
        dc.GetTextExtent(label, &w, &h);
        dc.DrawText(label, x - w / 2, y - h / 2);
    }

    void drawGpsMarker(wxDC& dc) {
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(wxBrush(wxColour(0, 120, 255, 160))); // light blue semi-transparent
        dc.DrawCircle(mousePos, 6);
        dc.SetBrush(wxBrush(wxColour(0, 120, 255))); // solid center
        dc.DrawCircle(mousePos, 2);
    }

    double km2d(double km) {
        return km / 111.32;
    }

    double m2d(double meters) {
        return meters / 111320.0;
    }

private:
    void MyCanvas::OnPaint(wxPaintEvent& event) {
        wxAutoBufferedPaintDC dc(this);
        dc.Clear();

        try {
            wxSize sz = GetSize();

            // Calcule offsetX et offsetY pour centrer le réseau
            offsetX = sz.GetWidth() / 2.0 - centerNetwork.x * scale;
            offsetY = sz.GetHeight() / 2.0 + centerNetwork.y * scale;

            auto transformX = [&](double x) {
                return GetSize().GetWidth() / 2.0 + (x - centerNetwork.x) * scale;
                };
            auto transformY = [&](double y) {
                return GetSize().GetHeight() / 2.0 - (y - centerNetwork.y) * scale;
                };


            // Trie par zIndex (optionnel selon ta structure)
            std::vector<TGeo> toDraw = geometries;
            std::sort(toDraw.begin(), toDraw.end(), [](const TGeo& a, const TGeo& b) {
                return a.style.zIndex < b.style.zIndex;
                });

            for (TGeo& geo : toDraw) {
                if (!geo.style.visible) continue;

                int alpha = geo.style.selected ? 255 : 100;

                wxPen pen(intToColor(geo.style.borderColor, alpha), geo.style.selected ? geo.style.borderWidth * 4 : geo.style.borderWidth);
                wxBrush brush(intToColor(geo.style.fillColor, alpha));
                dc.SetPen(pen);
                dc.SetBrush(brush);

                double labelX = 0, labelY = 0;

                if (geo.isSpot()) {
                    Spot s = geo.asSpot();
                    double x = transformX(s.x);
                    double y = transformY(s.y);
                    dc.DrawCircle(x, y, 4);
                    labelX = x;
                    labelY = y - 10;
                }
                else if (geo.isLine()) {
                    Line l = geo.asLine();
                    for (size_t i = 1; i < l.size(); ++i) {
                        Spot a = l.get(i - 1);
                        Spot b = l.get(i);
                        dc.DrawLine(transformX(a.x), transformY(a.y), transformX(b.x), transformY(b.y));
                    }
                    if (!l.empty()) {
                        Spot mid = l.get(l.size() / 2);
                        labelX = transformX(mid.x);
                        labelY = transformY(mid.y) - 6;
                    }

                    // Flèche pour sens unique
                    if (geo.style.oneWay) {
                        Spot a = l.get(0);
                        Spot b = l.get(1);
                        double ax = transformX(a.x), ay = transformY(a.y);
                        double bx = transformX(b.x), by = transformY(b.y);

                        double mx = (ax + bx) / 2.0;
                        double my = (ay + by) / 2.0;

                        double dx = bx - ax;
                        double dy = by - ay;
                        double len = std::hypot(dx, dy);
                        if (len > 0) {
                            dx /= len;
                            dy /= len;

                            double arrowSize = 8;
                            double px = mx - dy * arrowSize;
                            double py = my + dx * arrowSize;
                            double qx = mx + dy * arrowSize;
                            double qy = my - dx * arrowSize;

                            wxPoint pts[3] = {
                                wxPoint(mx + dx * arrowSize, my + dy * arrowSize),
                                wxPoint(px, py),
                                wxPoint(qx, qy)
                            };

                            dc.SetBrush(*wxBLACK_BRUSH);
                            dc.DrawPolygon(3, pts);
                        }
                    }
                }
                // autres types géométriques (polygon, multipolygon) …

                if (!geo.label.empty()) {
                    drawLabel(dc, wxString::FromUTF8(geo.label.c_str()), labelX, labelY);
                }
            }

            drawGpsMarker(dc);
        }
        catch (const std::exception& e) {
            wxMessageBox("Erreur pendant le dessin : " + wxString(e.what()), "Exception", wxOK | wxICON_ERROR);
        }
    }

};
