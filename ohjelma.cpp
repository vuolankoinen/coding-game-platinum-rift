#include <iostream> 
#include <string> 
#include <vector> 
#include <algorithm>

using namespace std;

// Pelitilanteen ym. olennaisen sisältävä olio - käytännössä kaikki tapahtuu tässä luokassa
struct Tiedot {
    Tiedot(
        int pelaajia,
        int omaId,
        vector<int> peliruudut, 
        vector<int> platinatuotot,
        vector<int> alkupisteet,
        vector<int> loppupisteet) ;
    // Suoraan luettavat pysyvät tiedot
    int playerCount;
    int myId;
    vector < int > idt;
    vector < int > platinatuotot;
    vector < int > loput;
    vector < int > alut;
    // Vuorottain vaihtuvat tiedot
    vector < int > omistajat;
    vector < int > vihut;
    vector < int > omat;
    int platinum;  // <- "todellinen" / 20, eli moneen yksikköön on varaa
    // API-functiot
    void kierroksen_alku(vector < int > omistajat, vector < int > vihut, vector < int > omat, int platinaa);
    void valmistele_ostot();
    void tee_ostot();
    void valmistele_liikkeet();
    void tee_liikkeet();
    bool ekat();
  private:
    int asento; // 0 -> laajennus, 1 -> puolustus, 2 -> aggressio
    int vro;
    vector < int > ostot;
    vector < int > liikkeet;
    // Strategisoinnin functiot
    vector<int> omistuksessa(vector<int> idt, int kenen);
    vector<int> tuottojrj(vector<int> a);
    vector<int> naapurit(int id);
    int satunnaissuunta(int id);
    vector<int> omia_joukkoja(vector<int> it);
    vector<int> omia_joukkoja();
    vector<int> houkuttelevimmat_vihukohteet(int id);
    void tyhjien_taytto();
    void rautaa_rajalle();
    bool altis(int id);
    bool uhattu(int id);
    vector<double> houkuttelevuudet();
    vector<int> houkutusjrj(vector<int> idt);
};

Tiedot::Tiedot(int p1, int p2, vector<int> p3, vector<int> p4, vector<int> p5, vector<int> p6) 
: asento(0), vro(0),
  playerCount(p1),
  myId(p2),
  idt(p3),
  platinatuotot(p4),
  loput(p5),
  alut(p6)
{
    idt = tuottojrj(idt);
}

void Tiedot::kierroksen_alku(vector < int > om, vector < int > vih, vector < int > o, int platinaa) {
    liikkeet.clear();
    ostot.clear();
    cerr << ++vro << ". vuoro alkaa." << endl;
    omistajat = om;
    vihut = vih;
    omat = o;
    platinum = platinaa;
//    idt = houkutusjrj(idt);
//    vector<double> koe2 = houkuttelevuudet();   // Kumpikin näistä tuntuu kaatavan ohjelman ajossa.
}

bool Tiedot::ekat() {
    // Sijoitetaan tuottavimpiin lopuilla varoilla
    vector<int> tyhj = omistuksessa(idt, -1);
    if (tyhj.size() >0 && platinum > 1) { // Houkuttelevimpaan laitetaan 2+, sillä näin voitetaan yleisin kanssapelaajien strategia laittaa kaikkiin houkutteleviin 1...
        int parhaaseen = rand() % 3;
        parhaaseen = parhaaseen -1 + playerCount;
        ostot.push_back(parhaaseen);
        ostot.push_back(tyhj[0]);
        platinum -= parhaaseen;
        cerr << " Parhaaseen meni " << parhaaseen <<endl;
        tyhj.erase(tyhj.begin());
    }
    if (tyhj.size() >0 && platinum > 0) {
        int apu = 0;
        while (platinum > 0 && apu < tyhj.size()) {
            ostot.push_back(1);
            ostot.push_back(tyhj[apu]);
            platinum -= 1;
            ++apu;
        }
    }
    return(false);
}

void Tiedot::valmistele_liikkeet() {
    vector <int> omia = omia_joukkoja();
    for (int tt = 0; tt < omia.size(); ++tt) {
        vector<int> kohteet = houkuttelevimmat_vihukohteet(omia[tt]);
        if (kohteet.size() == 0) { // Ei vallattavia ruutuja
            int sat = satunnaissuunta(omia[tt]);
            if (altis(omia[tt])) sat = -1; // Ei liikuta rajalta oman alueen suuntaan
            if (sat >= 0) {
                liikkeet.push_back(omat[omia[tt]]);
                liikkeet.push_back(omia[tt]);
                liikkeet.push_back(sat);
//                cerr << omat[omia[tt]] << " " << omia[tt] << " -> " << sat << " satunnaisliike" << endl;
            }
        } else { // Ainakin 1 vallattava ruutu
            liikkeet.push_back(omat[omia[tt]]);
            liikkeet.push_back(omia[tt]);
            liikkeet.push_back(kohteet[0]);
//            cerr << omat[omia[tt]] << " " << omia[tt] << " -> " << kohteet[0] << " valtaus" << endl;
        }
    }
}

void Tiedot::tee_liikkeet() {
    cerr << "Liikkeitä: " << liikkeet.size()/3 << endl;
    if (liikkeet.size() < 3) {
        cout << "WAIT";
    } else {
        for (int tt = 0; tt < liikkeet.size(); ++tt) 
            cout << liikkeet[tt] << " ";
    }
    cout << endl;
}

void Tiedot::valmistele_ostot() {
// Täytetään tyhjiä
    if (platinum)
        tyhjien_taytto();
    if (platinum)
        rautaa_rajalle();
}

void Tiedot::tyhjien_taytto() {
    vector<int> tyhj = omistuksessa(idt, -1);
    if (tyhj.size() >0 && platinum > 0) {
        int apu = 0;
        while (platinum > 0 && apu < tyhj.size()) {
            ostot.push_back(1);
            ostot.push_back(tyhj[apu]);
            platinum -= 1;
            ++apu;
        }
    }
}

void Tiedot::rautaa_rajalle(){
    vector<int> omat = omistuksessa(idt, myId);
    vector<int> raja;
    for (int tt = 0; tt < omat.size(); ++tt) { 
        if (houkuttelevimmat_vihukohteet(omat[tt]).size() >= 0) // Vallattavia alueita
            for (int ss = 1; ss <= 5; ++ss)
                raja.push_back(omat[tt]);
        if (altis(omat[tt])) // Raja
            for (int ss = 1; ss <= 1; ++ss)
                raja.push_back(omat[tt]);
        if (uhattu(omat[tt])) // Raja, jonka takana vihuja
            for (int ss = 1; ss <= 20; ++ss)
                raja.push_back(omat[tt]);
    }
    if (raja.size() == 0 || platinum == 0)
        return;
    int rasi = raja.size();
    while (raja.size() > platinum) {
        int mx = raja.size();
        int r = rand() % mx;
        raja.erase(raja.begin()+r);
        rasi = raja.size();

    }
    unsigned kuhunkin = platinum / raja.size();
    kuhunkin = (kuhunkin == 0) ? 1 : kuhunkin;
    int apu = 0;
    while (platinum > 0 && apu < raja.size()) {
        ostot.push_back(kuhunkin);
        ostot.push_back(raja[apu]);
        platinum -= 1;
        ++apu;
    }
}

void Tiedot::tee_ostot() {
    cerr << "Ostoja: " << ostot.size()/2 << endl;
    if (ostot.size() < 2) {
        cout << "WAIT";
    } else {
        for (int tt = 0; tt < ostot.size(); ++tt) 
            cout << ostot[tt] << " ";
    }
    cout << endl;
}

// Palauttaa listan tietyn pelaajan alueista (kenen = -1 -> vapaista)
vector<int> Tiedot::omistuksessa(vector<int> idt, int kenen) {
    vector<int> tulos;
    for (int it = 0; it < idt.size(); ++it) {
        if (omistajat[idt[it]] == kenen) tulos.push_back(idt[it]);
    }
    return(tulos);
}

// Järjestää alueiden id:t tuottojärjestykseen - tarvitaan siis vain kerran pelissä.
vector<int> Tiedot::tuottojrj(vector<int> vapaat) {
    if (vapaat.size() < 2) return(vapaat);
    int mx = 0;
    vector<int> lisattavat, hannille, AB;
    for (int tt =0; tt < vapaat.size(); ++tt) {
        mx = max(mx, platinatuotot[vapaat[tt]]);
    }
    for (int tt =0; tt < vapaat.size(); ++tt) {
        if (mx == platinatuotot[vapaat[tt]]) {
            lisattavat.push_back(vapaat[tt]);
        } else {
            hannille.push_back(vapaat[tt]);
        }
    }
    hannille = tuottojrj(hannille);
    AB.reserve( lisattavat.size() + hannille.size() ); // preallocate memory
    AB.insert( AB.end(), lisattavat.begin(), lisattavat.end() );
    AB.insert( AB.end(), hannille.begin(), hannille.end() );
    return(AB);
}

vector<int> Tiedot::houkutusjrj(vector<int> vapaat) {
    cerr << " 0 ";
    if (vapaat.size() < 2) return(vapaat);
    cerr << " 1 ";
    vector<double> houkutus = houkuttelevuudet();
    for (int tt = 0; tt < vapaat.size(); ++tt) {
        int ss = rand() % 60;
        houkutus[tt] = ss / 10.0;
    }
    double mx = 0.0;
    cerr << " 2 ";
    vector<int> lisattavat, hannille, AB;
    for (int tt = 0; tt < vapaat.size(); ++tt) {
        cerr << " 3 ";
        mx = max(mx, houkutus[vapaat[tt]]);
    }
    for (int tt =0; tt < vapaat.size(); ++tt) {
        if (mx == houkutus[vapaat[tt]]) {
            lisattavat.push_back(vapaat[tt]);
        } else {
            hannille.push_back(vapaat[tt]);
        }
    }
    hannille = houkutusjrj(hannille);
    AB.reserve( lisattavat.size() + hannille.size() ); // preallocate memory
    AB.insert( AB.end(), lisattavat.begin(), lisattavat.end() );
    AB.insert( AB.end(), hannille.begin(), hannille.end() );
    return(AB);
}

// Mihin alueelta pääsee?
vector<int> Tiedot::naapurit(int id) {
    vector<int> tulos;
    for (int tt = 0; tt < alut.size(); ++tt) {
        if (alut[tt] == id)
            tulos.push_back(loput[tt]);
        if (loput[tt] == id)
            tulos.push_back(alut[tt]);
    }
    return(tulos);    
}

int Tiedot::satunnaissuunta(int id) {
    vector<int> na = naapurit(id);
    if (na.size() == 0) return(-1);
    int mx = na.size();
    int r = rand() % mx;
    int tulos = na[r];
    return(tulos);
}

vector<int> Tiedot::omia_joukkoja(vector<int> idt) {
    vector<int> tulos;
    for (int tt = 0; tt < idt.size(); ++tt) 
        if (omat[idt[tt]]>0)
            tulos.push_back(idt[tt]);
    return(tulos);
}

vector<int> Tiedot::omia_joukkoja() {
    return(omia_joukkoja(idt));
}

vector<int> Tiedot::houkuttelevimmat_vihukohteet(int id) {
    vector<int> kokelaat = naapurit(id);
    vector<int> kokelaat2;
    for (int tt = 0; tt < kokelaat.size(); ++tt) {
        if (omistajat[kokelaat[tt]] != myId)
            kokelaat2.push_back(kokelaat[tt]);
    }
    kokelaat = tuottojrj(kokelaat2);
    return(kokelaat);
}

bool Tiedot::altis(int id) {
    bool tulos = false;
    vector<int> naaps = naapurit(id);
    for (int tt = 0; tt < naaps.size(); ++tt) 
        if (omistajat[naaps[tt]] != myId) 
            tulos = true;
    return(tulos);
}

bool Tiedot::uhattu(int id) {
    bool tulos = false;
    vector<int> naaps = naapurit(id);
    for (int tt = 0; tt < naaps.size(); ++tt) 
        if (vihut[naaps[tt]] > 0)
            tulos = true;
    return(tulos);
}

// Kuinka haluttavia eri ruudut ovat 
vector<double> Tiedot::houkuttelevuudet() {
    int etaisyys = 2; // Kuinka kaukaa muiden ruutujen vaikutus kantaa?
    double kerroin = 0.2; // Kuinka iso osuus naapureiden haluttavuudesta siirtyy eteenpäin?
    vector<double> tulos(idt.size());
    for (int tt = 0; tt < tulos.size(); ++tt) {
        double arvo;
        if (omistajat[tt] != myId)
            arvo += 1.0;
        arvo += platinatuotot[tt];
        if (omistajat[tt] = myId)
            arvo = arvo * 0.2;
    }
    for (int d = 0; d < etaisyys; ++d) {
        vector<double> apu(idt.size());
            for (int ss = 0; ss < tulos.size(); ++ss) {
                vector<int> naa = naapurit(ss);
                int valitulos = 0;
                for (int uu = 0; uu < naa.size(); ++uu) {
                    valitulos = valitulos + kerroin * tulos[naa[ss]];
                }
                apu[ss] = valitulos;
            }
        tulos = apu;
    }
    return(tulos);
}



int main() {
      int playerCount; // the amount of players (2 to 4)
      int myId; // my player ID (0, 1, 2 or 3)
      int zoneCount; // the amount of zones on the map
      int linkCount; // the amount of links between all zones
      cin >> playerCount >> myId >> zoneCount >> linkCount;
      cin.ignore();
      
      
      // Omat muuttujat
      vector < int > idt(zoneCount);
      vector < int > platinatuotot(zoneCount);
      vector < int > loput(linkCount);
      vector < int > alut(linkCount);
      vector < int > omistajat(zoneCount);
      vector < int > vihut(zoneCount);
      vector < int > omat(zoneCount);
      bool eka_kierros = true;

      for (int i = 0; i < zoneCount; i++) {
         //        int zoneId; // this zone's ID (between 0 and zoneCount-1)
         //        int platinumSource; // the amount of Platinum this zone can provide per game turn
         //        cin >> zoneId >> platinumSource; cin.ignore(); // alkuperäinen
         cin >> idt[i] >> platinatuotot[i];
         cin.ignore();
      }
      for (int i = 0; i < linkCount; i++) {
         //        int zone1;
         //        int zone2;
         //        cin >> zone1 >> zone2; cin.ignore();
         cin >> alut[i] >> loput[i];
         cin.ignore();
      }
      Tiedot tiedot(playerCount, myId, idt, platinatuotot, alut, loput);
      
      // game loop
    while (1) {
        int platinum; // my available Platinum
        cin >> platinum;
        cin.ignore();
        for (int i = 0; i < zoneCount; i++) {
            int zId; // this zone's ID
            int ownerId; // the player who owns this zone (-1 otherwise)
            int podsP0; // player 0's PODs on this zone
            int podsP1; // player 1's PODs on this zone
            int podsP2; // player 2's PODs on this zone (always 0 for a two player game)
            int podsP3; // player 3's PODs on this zone (always 0 for a two or three player game)
            cin >> zId >> ownerId >> podsP0 >> podsP1 >> podsP2 >> podsP3;
            cin.ignore();
            omistajat[zId] = ownerId;
            if (myId < 2) {
                    omat[zId] = (myId == 0) ? podsP0 : podsP1;
                } else {
                    omat[zId] = (myId == 2) ? podsP2 : podsP3;
                }
            vihut[zId] = podsP1 + podsP2 + podsP3 + podsP0 - omat[zId];
        }
        tiedot.kierroksen_alku(omistajat, vihut, omat, platinum/20);
        if (eka_kierros) eka_kierros = tiedot.ekat();
        tiedot.valmistele_liikkeet();
        tiedot.tee_liikkeet();
        tiedot.valmistele_ostot();
        tiedot.tee_ostot();
    }
}
