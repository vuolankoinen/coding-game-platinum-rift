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
    vector<int> omia_joukkoja(vector<int> idt);
    int houkuttelevin_vihukohde(int id);
    void tyhjien_taytto();
    void rautaa_rajalle();
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
    cerr << "Vektorien pituudet: omistajat " << om.size() << endl;
    omistajat = om;
    vihut = vih;
    omat = o;
    platinum = platinaa;
}

void Tiedot::valmistele_liikkeet() {

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
    if (tyhj.size() >0 && platinum > 1) { // Houkuttelevimpaan laitetaan 2, sillä näin voitetaan yleisin kanssapelaajien strategia laittaa kaikkiin houkutteleviin 1...
        ostot.push_back(2);
        ostot.push_back(tyhj[0]);
        platinum -= 2;
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
}

void Tiedot::rautaa_rajalle(){
    vector<int> omat = omistuksessa(idt, myId);
    vector<int> raja;
    for (int tt = 0; tt < omat.size(); ++tt) 
        if (houkuttelevin_vihukohde(omat[tt]) >= 0)
            raja.push_back(omat[tt]);
    if (raja.size() == 0 || platinum == 0)
        return;
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
//        cerr << " it " << it << " alue " << idt[it] /*<< " omistaja " << omistajat[idt[it]];*/ << " kenen " << kenen << endl;
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

vector<int> Tiedot::naapurit(int id) {
    vector<int> tulos;
    for (int tt = 0; tt < alut.size(); ++tt) {
        if (alut[tt] == id)
            tulos.push_back(loput[tt]);
    }
    return(tulos);    
}

int Tiedot::satunnaissuunta(int id) {
    vector<int> na = naapurit(id);
    int mx = na.size() + 1;
    return(na[rand() % mx]);
}

vector<int> Tiedot::omia_joukkoja(vector<int> idt) {
    vector<int> tulos;
    for (int tt = 0; tt < idt.size(); ++tt) 
        if (omat[idt[tt]]>0)
            tulos.push_back(idt[tt]);
    return(tulos);
}

int Tiedot::houkuttelevin_vihukohde(int id) {
    vector<int> kokelaat = naapurit(id);
    vector<int> kokelaat2;
    for (int tt = 0; tt < kokelaat.size(); ++tt)
        if (omistajat[kokelaat[tt]] != myId)
            kokelaat2.push_back(kokelaat[tt]);
    if (kokelaat2.size() == 0) {
//        cerr << "Ruudulla " << id << " ei ole vihunaapureita!" << endl;
        return(-1);
    }
    int tulos = kokelaat2[0];
    for (int tt = 0; tt < kokelaat2.size(); ++tt)
        if (platinatuotot[kokelaat2[tt]] > platinatuotot[tulos])
            tulos = kokelaat2[tt];
    return(tulos);
}


int main() {
      int playerCount; // the amount of players (2 to 4)
      int myId; // my player ID (0, 1, 2 or 3)
      int zoneCount; // the amount of zones on the map
      int linkCount; // the amount of links between all zones
      cin >> playerCount >> myId >> zoneCount >> linkCount;
      cerr << myId << " my id" << endl;
      cin.ignore();
      
      
      // Omat muuttujat
      vector < int > idt(zoneCount);
      vector < int > platinatuotot(zoneCount);
      vector < int > loput(linkCount);
      vector < int > alut(linkCount);
      vector < int > omistajat(zoneCount);
      vector < int > vihut(zoneCount);
      vector < int > omat(zoneCount);
      vector < int > tyhj;
      vector<int> omikset;
      int raja;
      int wait; // Eikö tullut liikkeitä tällä kierroksella?
      int laht;
      int kohd;
      vector<int> ostot;
      vector<int> liikkeet;

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

         // Write an action using cout. DON'T FORGET THE "<< endl"
         // To debug: cerr << "Debug messages..." << endl;

         // first line for movement commands, second line for POD purchase (see the protocol in the statement for details)
        cerr << "1! ";
        tiedot.valmistele_liikkeet();
        cerr << "2! ";
        tiedot.tee_liikkeet();
        cerr << "3! ";
        tiedot.valmistele_ostot();
        cerr << "4! ";
        tiedot.tee_ostot();
        cerr << "5! " << endl;
    }
}
