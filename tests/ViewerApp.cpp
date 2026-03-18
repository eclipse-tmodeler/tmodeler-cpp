#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include "modelsTM.hpp" // Inclut Spot, Line, TPolygon, etc.
#include "MyCanvas.h"


wxDEFINE_EVENT(EVT_CITY_LOADED, wxThreadEvent);

class MyFrame : public wxFrame {
public:
    MyFrame()
        : wxFrame(nullptr, wxID_ANY, "TGeo Viewer", wxDefaultPosition, wxSize(800, 600)) {

        canvas = new MyCanvas(this);
        Centre();

        Bind(EVT_CITY_LOADED, &MyFrame::OnCityLoaded, this);

        // ⏳ Chargement asynchrone
        std::thread loader([this]() {
            try {
                testInitTM([this]() {
                    auto data = doualaCityFromModel();
                    wxThreadEvent evt(EVT_CITY_LOADED);
                    evt.SetPayload(data);
                    wxQueueEvent(this, evt.Clone());
                });
            }
            catch (const std::exception& e) {
                std::string d = e.what();
                wxLogError("Erreur chargement : %s", e.what());
            }
            });
        loader.detach();
    }

private:
    MyCanvas* canvas = nullptr;

    void OnCityLoaded(wxThreadEvent& event) {
        canvas->base_geometries = event.GetPayload<std::vector<TGeo>>();
        canvas->geometries = canvas->base_geometries;
        canvas->Refresh();
        wxLogMessage("Donnees chargees !");
    }
};

class MyApp : public wxApp {
public:
    virtual bool OnInit() override {
        MyFrame* frame = new MyFrame();
        frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(MyApp);
