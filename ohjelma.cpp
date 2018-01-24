#include <iostream> 
#include <string> 
#include <vector> 
#include <set> 
#include <algorithm>

using namespace std;

/* 
Kehitysajatuksia:
    - peliasennot
        - esim. ADT niitä säätelemään
    - tuottavien ruutujen omistussuhteiden (ja siten tulojen) tarkkaileminen
    - mantereiden tarkkaileminen
        - manner-luokka?
        - vallattujen mantereiden tiputtaminen pohdinnoista
        - sillanpään varmistaminen voittoon tarvittavilla mantereilla
            - viimeiseen tyhjään laitetaan joukkoja
            - neljän poppoolla voi ajella tuottaviin ruutuihin
            - pienemmällä poppoolla väistellään pitkin maaseutua
        - kullekin mantereelle oma asento?
            - normaali
            - puhdistetaan
            - pidetään sillanpäätä auki
            - vallataan
    - huokuttelevuuksien laskemiseen naapureiden vaikutukset kaikille etäisyyksille (mantereittain)
*/

/***********  MANNER-luokka ***********/
/*************************************/
// Toisiinsa yhteydessä olevien ruutujen joukko
struct Manner {
    // Pseudo-konstruktori, palauttaa mantereen alueet
    set<int> alusta(int id, vector<int> alut, vector<int> loput);
    set<int> idt;
    vector<int> alut;
    vector<int> loput;
    set<int> alueet() const {return(idt);}

};
set<int> Manner::alusta(int id, vector<int> kaikki_alut, vector<int> kaikki_loput) {
    set<int> tulos, uudet;
    tulos.insert(id);
    uudet.insert(id);
    while (uudet.size()>0) {
        set<int> apu;
        for (std::set<int>::iterator it=uudet.begin(); it!=uudet.end(); ++it) {
            for (int ss = 0; ss < kaikki_alut.size(); ++ss) {
                if (*it == kaikki_alut[ss] && tulos.count(kaikki_loput[ss])==0) {
                    tulos.insert(kaikki_loput[ss]);
                    apu.insert(kaikki_loput[ss]);
                }
                if (*it == kaikki_loput[ss] && tulos.count(kaikki_alut[ss])==0) {
                    tulos.insert(kaikki_alut[ss]);
                    apu.insert(kaikki_alut[ss]);
                }
            }
        }
        uudet = apu;
    }
    idt = tulos;
    vector<int> al, lop;
    for (int tt = 0; tt < kaikki_alut.size(); ++tt)
        if (idt.count(kaikki_alut[tt]) == 1 || idt.count(kaikki_loput[tt]) == 1) {
            al.push_back(kaikki_alut[tt]);
            lop.push_back(kaikki_loput[tt]);
        }
    alut = al;
    loput = lop;
    return (tulos);
}
bool operator < ( Manner const &o, Manner const &v) {
    if (v.idt.size() < o.idt.size())
        return(true); // Isommat mantereet alkuun
    return(*v.idt.begin() < *o.idt.begin()); // Ratkaisee kaikki tasurit
}


/***********  INFOT-luokka ***********/
/*************************************/
// Pelitilanteeseen liittyvät tiedot säilyttävä ja niitä manipuloiva luokka
class Infot {
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
    int vro;
    // Talteen laskettavat tiedot
    vector <int> houkuttelevuudet;
    set<Manner> mantereet;
    // Funktioita datan pyörittely**yn
    vector<int> jrj(vector<int> ehdokkaat, vector<int> ehdokkaiden_arvot);
    vector<int> omistukset(vector<int> ehdokkaat, int kenen);
    vector<int> laske_houkuttelevuudet(vector<int> ehdokkaat);
    bool uhattunako(int id);
    bool altisko(int id);
    bool valmis_mannerko(Manner manner);    
  public:
    // Konstruktorit
    Infot();
    Infot ( int pelaajia, int omaId,
            vector<int> peliruudut, vector<int> platinatuotot,
            vector<int> alkupisteet, vector<int> loppupisteet);
    // API-funktiot
    void kierroksen_alku(vector < int > omistajat, vector < int > vihut, vector < int > omat, int platinaa);
    bool eka_kierrosko();
    vector<int> parhaat_alueet(int kenen); // -1: tuottavat alueet, 0: alueet, joihin voi sijoittaa joukkoja
    vector<int> joukkoja(int kenen); // 0 omia, 1 muiden
    int varaa() {return(platinum);}
    int pelaajia() {return(playerCount);}
    int montako_omaa(int id) {return(omat[id]);}
    vector<int> suunnat(int id, int moneenko_suuntaan);
    int maksa(int paljonko, int mihin);
    vector<int> naapurit(int id);
    int satunnaissuunta(int id);
    int houkutus(int id) {return(houkuttelevuudet[id]);}
    void poista_valmiit_mantereet();
};

Infot::Infot() {}
Infot::Infot(int pelaajia, int id, vector<int> it, vector<int> tuotot, vector<int> lop, vector<int> alk) 
:  vro(0),
  playerCount(pelaajia),
  myId(id),
  idt(it),
  platinatuotot(tuotot),
  loput(lop),
  alut(alk)
{
    idt = jrj(idt, platinatuotot);
    set<Manner> uusi_kokoelma;
    vector<int> apu;
    vector<int> jaljella(idt);
    while (jaljella.size() > 0) {
        apu.clear();
        Manner uusi;
        set<int> menneet = uusi.alusta(jaljella[0], alut, loput);
        uusi_kokoelma.insert(uusi);
        for (int tt = 0; tt < jaljella.size(); ++tt)
            if (menneet.count(jaljella[tt])==0)
                apu.push_back(jaljella[tt]);
        jaljella = apu;
    }
    mantereet = uusi_kokoelma;
    cerr << "Mantereita luotu " << uusi_kokoelma.size() << ", niiden koot: " <<endl;
    for (set<Manner>::iterator it=mantereet.begin(); it != mantereet.end(); it++)
        cerr << (*it).idt.size() << " ";
    cerr << endl;
}

void Infot::kierroksen_alku(vector < int > om, vector < int > vih, vector < int > o, int platinaa) {
    cerr << ++vro << ". vuoro alkaa." << endl;
    omistajat = om;
    vihut = vih;
    omat = o;
    platinum = platinaa;
    houkuttelevuudet = laske_houkuttelevuudet(idt);
    poista_valmiit_mantereet();
}

bool Infot::eka_kierrosko() {
    return(vro==1);
}
vector<int> Infot::parhaat_alueet(int kenen) {
    if (kenen == 0)
        kenen = myId;
    vector<int> tulos = omistukset(idt, kenen);
    if (kenen == myId) { // Lisätään omiin vielä tyhjät
        vector<int> apu = omistukset(idt, -1);
        tulos.insert(tulos.end(), apu.begin(), apu.end());
    }
    if (kenen == -1) { // Poistetaan tyhjistä tuottamattomat
        vector<int> apu;
        for (int tt = 0; tt < tulos.size(); ++tt)
            if (platinatuotot[tulos[tt]]>1)
                apu.push_back(tulos[tt]);
        tulos = apu;
    }
    return(jrj(tulos, houkuttelevuudet));
}
vector<int> Infot::omistukset(vector<int> ehdokkaat, int kenen) {
    vector<int> tulos;
    for (int tt = 0; tt < ehdokkaat.size(); ++tt)
        if (omistajat[ehdokkaat[tt]] == kenen)
            tulos.push_back(ehdokkaat[tt]);
    return(tulos);
}
vector<int> Infot::laske_houkuttelevuudet(vector<int> ehd) {
    vector<int> tulos(platinatuotot.size());
    for (int tt = 0; tt < ehd.size(); ++tt) {
        tulos[ehd[tt]] = 100*platinatuotot[ehd[tt]]; // Indeksöinti olennainen - pitää tarkistaa laske_houkuttelevuudet samalla indeksöinnillä!
        if (omistajat[ehd[tt]] != myId)
            tulos[ehd[tt]] = tulos[ehd[tt]] * 2 + 300;
        if (omistajat[ehd[tt]] == myId) {
            tulos[ehd[tt]] = tulos[ehd[tt]] / 20;
            if (uhattunako(ehd[tt]))
                tulos[ehd[tt]] = tulos[ehd[tt]] * 25 + 150;
            if (altisko(ehd[tt]))
                tulos[ehd[tt]] = tulos[ehd[tt]] * 10 + 50;
        }
    }
    // Alueeseen yhteydessä olevien alueiden vaikutus
    int etaisyys = 2; // Kuinka kaukaa
    int osuus = 20; // Kuinka monta prosenttia naapureiden arvosta siirtyy eteenpäin.
    vector<int> lisat(platinatuotot.size());
    for (int d = 0; d < etaisyys; ++d) {
        for (int tt = 0; tt < idt.size(); ++tt) {
            vector<int> naap = naapurit(idt[tt]);
            for (int nn = 0; nn < naap.size(); ++nn) {
                lisat[idt[tt]] += tulos[naap[nn]] * osuus;
            }
        }
        for (int ss = 0; ss < tulos.size(); ++ss)
            tulos[ss] += lisat[ss] /100;
    }
    return(tulos);
}
int Infot::maksa(int x, int id) {
    platinum -= x;
    houkuttelevuudet[id] = 0;
    return(platinum);
}
vector<int> Infot::jrj(vector<int> ehdokkaat, vector<int> arvot) {
    if (ehdokkaat.size() < 2) return(ehdokkaat);
    int mx = 0;
    vector<int> lisattavat, hannille, tulos;
    for (int tt =0; tt < ehdokkaat.size(); ++tt) {
        mx = max(mx, arvot[ehdokkaat[tt]]);
    }
    for (int tt =0; tt < ehdokkaat.size(); ++tt) {
        if (mx == arvot[ehdokkaat[tt]]) {
            lisattavat.push_back(ehdokkaat[tt]);
        } else {
            hannille.push_back(ehdokkaat[tt]);
        }
    }
    hannille = jrj(hannille, arvot);
    tulos.reserve( lisattavat.size() + hannille.size() ); // preallocate memory
    tulos.insert( tulos.end(), lisattavat.begin(), lisattavat.end() );
    tulos.insert( tulos.end(), hannille.begin(), hannille.end() );
    return(tulos);
}
vector<int> Infot::joukkoja(int kenen) { // 0 omia, 1 muiden
    vector<int> tulos;
    if (kenen)  {
        for (int tt = 0; tt < idt.size(); ++tt) 
            if (vihut[idt[tt]] > 0)
                tulos.push_back(idt[tt]);
    } else {
        for (int tt = 0; tt < idt.size(); ++tt) 
            if (omat[idt[tt]] > 0) 
                tulos.push_back(idt[tt]);
    }
    return(tulos);
}
vector<int> Infot::suunnat(int id, int moneenko_suuntaan) {
    vector<int> tulos;
    vector<int> ehdokkaat = naapurit(id);
    ehdokkaat.push_back(id); // Myös tämänhetkinen ruutu (eli liikkumattomuus) on otettava huomioon vaihtoehdoissa.
    while (ehdokkaat.size() < moneenko_suuntaan)
        ehdokkaat.push_back(id);
    reverse(ehdokkaat.begin(), ehdokkaat.end()); // Suositaan paikallaan pysymistä ärsyttävyyden minimoimiseksi.
    ehdokkaat = jrj(ehdokkaat, houkuttelevuudet);
    for (int tt = 0; tt < moneenko_suuntaan; ++tt)
    {
        tulos.push_back(ehdokkaat[tt]);
        houkuttelevuudet[ehdokkaat[tt]] /= 2; // Jonoon lisätyn liikkeen johonkin ruutuun tulee heikentää sen houkuttelevuutta.
    }
    return(tulos);
}
vector<int> Infot::naapurit(int id) {
    vector<int> tulos;
    for (int tt = 0; tt < alut.size(); ++tt) {
        if (alut[tt] == id)
            tulos.push_back(loput[tt]);
        if (loput[tt] == id)
            tulos.push_back(alut[tt]);
    }
    return(tulos);        
}
bool Infot::uhattunako(int id) {
    bool tulos;
    tulos = false;
    vector<int> naaps = naapurit(id);
    for (int tt = 0; tt < naaps.size(); ++tt)
        if (vihut[naaps[tt]]>0)
            tulos = true;            
    return(tulos);
}
int Infot::satunnaissuunta(int id) {
    vector<int> na = naapurit(id);
    if (na.size() == 0) return(-1);
    int mx = na.size();
    int r = rand() % mx;
    int tulos = na[r];
    return(tulos);
}
bool Infot::altisko(int id) {
    bool tulos = false;
    vector<int> naaps = naapurit(id);
    for (int tt = 0; tt < naaps.size(); ++tt) 
        if (omistajat[naaps[tt]] != myId) 
            tulos = true;
    return(tulos);
}
void Infot::poista_valmiit_mantereet() {
    for (set<Manner>::iterator it = mantereet.begin(); it != mantereet.end(); ++it) {
        if (valmis_mannerko(*it)) {
            set<int> poistettavat = (*it).alueet();
            vector<int> karsitut_idt, karsitut_alut, karsitut_loput;
            for (int tt = 0; tt < idt.size(); ++tt)
                if (poistettavat.count(idt[tt]) == 0)
                    karsitut_idt.push_back(idt[tt]);
            for (int tt = 0; tt < alut.size(); ++tt)
                if (poistettavat.count(alut[tt]) == 0 && poistettavat.count(loput[tt]) == 0) {
                    karsitut_alut.push_back(alut[tt]);
                    karsitut_loput.push_back(loput[tt]);
                }
            idt = karsitut_idt;
            alut = karsitut_alut;
            loput = karsitut_loput;
        }
    }
}
bool Infot::valmis_mannerko(Manner manner) {
    set<int> alueet = manner.alueet();
    set<int>::iterator it = alueet.begin();
    int ekan_omistaja = omistajat[*it];
    bool tulos = (ekan_omistaja == -1) ? false : true;
    while (it != alueet.end() && tulos == true) {
        if (omistajat[*it] != ekan_omistaja || vihut[*it]>0)
            tulos = false;
        ++it;
    }
    return(tulos);
}

/***********  MEKANIIKKA-luokka ***********/
/******************************************/
// Pelimekaniikkaa pyörittävä luokka
struct Mekaniikka {
    Mekaniikka(Infot in);
    // API-functiot
    void liikkeet();
    void ostot();
  private:
    Infot info;
    vector<int> valmistele_liikkeet();
    void tee_liikkeet(vector<int> liik);
    vector<int> valmistele_ostot();
    void tee_ostot(vector<int> ost);
    void ekat_ostot(vector<int> & ost);
    void tyhjien_taytto(vector<int> & ost);
    void rautaa_rajalle(vector<int> & ost);
} ;

Mekaniikka::Mekaniikka(Infot in) {
    info = in;
}
void Mekaniikka::liikkeet() {
    tee_liikkeet(valmistele_liikkeet());
}
void Mekaniikka::ostot() {
    tee_ostot(valmistele_ostot());
}
vector<int> Mekaniikka::valmistele_liikkeet() {
    vector<int> tulos;
    vector <int> alueet = info.joukkoja(0); // Omia joukkoja sisältävät ruudut
    for (int tt = 0; tt < alueet.size(); ++tt) {
        // Yli neljän ryppäitä kannattaa hajottaa
        int moneenko_suuntaan = info.montako_omaa(alueet[tt]) / 4 + 1;
        vector<int> suun = info.suunnat(alueet[tt], moneenko_suuntaan);
        for (int ss = 0; ss < moneenko_suuntaan-1; ++ss) 
            if (suun[ss] != alueet[tt]) {
                tulos.push_back(4);
                tulos.push_back(alueet[tt]);
                tulos.push_back(suun[ss]);
        }
        if (suun[moneenko_suuntaan-1] != alueet[tt]) {
            int jaaneet = info.montako_omaa(alueet[tt]) % 4;
            if (jaaneet) {
                tulos.push_back(jaaneet);
                tulos.push_back(alueet[tt]);
                tulos.push_back(suun[moneenko_suuntaan-1]);
            }
        }
    }
    return(tulos);
}
void Mekaniikka::tee_liikkeet(vector<int> liik) {
        cerr << "Liikkeitä: " << liik.size()/3 << endl;
    if (liik.size() < 3) {
        cout << "WAIT";
    } else {
        for (int tt = 0; tt < liik.size(); ++tt) 
            cout << liik[tt] << " ";
    }
    cout << endl;
}
vector<int> Mekaniikka::valmistele_ostot() {
    vector<int> tulos;
    if (info.eka_kierrosko()) {
        ekat_ostot(tulos);
    }
    if (info.varaa())
        tyhjien_taytto(tulos);
    if (info.varaa())
        rautaa_rajalle(tulos);
    return(tulos);
}
void Mekaniikka::tyhjien_taytto(vector<int> & ost) {
    vector<int> tyhj = info.parhaat_alueet(-1);
    if (tyhj.size() >0 && info.varaa() > 0) {
        int apu = 0;
        while (info.varaa() > 0 && apu < tyhj.size()) {
            ost.push_back(1);
            ost.push_back(tyhj[apu]);
            info.maksa(1, tyhj[apu]);
            ++apu;
        }
    }
}
void Mekaniikka::rautaa_rajalle(vector<int> & ost) {
    vector<int> raja = info.parhaat_alueet(0);
    if (raja.size()==0) return;
    int kynnys;
    for (int tt = 0; tt < raja.size(); ++tt) 
        kynnys += info.houkutus(raja[tt]);
    kynnys = kynnys / 2 / raja.size();
    while (raja.size() > 2 && info.houkutus(raja[raja.size()-1]) < kynnys)
        raja.pop_back();
    unsigned kuhunkin = info.varaa() / raja.size();
    kuhunkin = (kuhunkin == 0) ? 1 : kuhunkin;
    int apu = 0;
    while (info.varaa() > 0 && apu < raja.size()) {
        ost.push_back(kuhunkin);
        ost.push_back(raja[apu]);
        info.maksa(1, raja[apu]);
        ++apu;
    }
}
void Mekaniikka::tee_ostot(vector<int> ost) {
        cerr << "Ostoja: " << ost.size()/2 << endl;
    if (ost.size() < 2) {
        cout << "WAIT";
    } else {
        for (int tt = 0; tt < ost.size(); ++tt)
            cout << ost[tt] << " ";
    }
    cout << endl;
}
void Mekaniikka::ekat_ostot(vector<int> & ost) {
    vector<int> tyhj = info.parhaat_alueet(-1);
    int tt = 0;
    int r = 1 + rand() % 4;
    while (tt < tyhj.size() && tt < r && info.varaa() > 0) { // Houkuttelevimpaan laitetaan 2+, sillä näin voitetaan yleisin kanssapelaajien strategia laittaa kaikkiin houkutteleviin 1...
        int parhaaseen = rand() % 3;
        parhaaseen = parhaaseen -1 + info.pelaajia();
        parhaaseen = min(4, parhaaseen);
        ost.push_back( (parhaaseen <= info.varaa()) ? parhaaseen:info.varaa() );
        ost.push_back(tyhj[tt]);
        info.maksa((parhaaseen <= info.varaa()) ? parhaaseen:info.varaa(), tyhj[tt]);
        ++tt;
    }
    while (tt < tyhj.size() && info.varaa() > 0) {
        ost.push_back(1);
        ost.push_back(tyhj[tt]);
        info.maksa(1, tyhj[tt]);
        tt = tt + 1 + rand() % 3;
    }
}

/***********  pääsilmukka ***********/
/************************************/

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

      for (int i = 0; i < zoneCount; i++) {
         //        int zoneId; // this zone's ID (between 0 and zoneCount-1)
         //        int platinumSource; // the amount of Platinum this zone can provide per game turn
         cin >> idt[i] >> platinatuotot[i];
         cin.ignore();
      }
      for (int i = 0; i < linkCount; i++) {
         cin >> alut[i] >> loput[i];
         cin.ignore();
      }

      Infot infot(playerCount, myId, idt, platinatuotot, alut, loput);
      
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
        infot.kierroksen_alku(omistajat, vihut, omat, platinum/20);
        Mekaniikka mek(infot);
        mek.liikkeet();
        mek.ostot();
    }
}
