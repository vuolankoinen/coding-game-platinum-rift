#include <iostream> 
#include <vector> 
#include <set> 
#include <map> 
#include <utility> 
#include <algorithm>

void t(std::vector<int> vektori) {
    std::cerr << vektori.size() << " alkiota: ";
    for (int tt = 0; tt < vektori.size();++tt)
        std::cerr << vektori[tt] << " ";
    std::cerr << std::endl;
}
void t(std::set<int> setti) {
    std::cerr << setti.size() << " alkiota: ";
    for (std::set<int>::iterator it = setti.begin(); it != setti.end(); ++it)
        std::cerr << *it << " ";
    std::cerr << std::endl;
}
void t(std::map<int, int> parit) {
    std::cerr << parit.size() << " alkiota: ";
    for (std::map<int, int>::iterator it = parit.begin(); it != parit.end(); ++it)
        std::cerr << std::get<0>(*it) << ":"<< std::get<1>(*it) << "  ";
    std::cerr << std::endl;
}

/* 
Kehitysajatuksia:
    - peliasennot
        - esim. ADT niitä säätelemään - tosin käsin arvioitaessa ja säädettäessä tämän hyödyllisyys on kyseenalainen
    - tuottavien ruutujen omistussuhteiden (ja siten tulojen) tarkkailestd::minen
    - mantereiden tarkkailestd::minen
        - sillanpään varmistastd::minen voittoon tarvittavilla mantereilla
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
    std::set<int> alusta(int id, std::vector<int> alut, std::vector<int> loput);
    // arvot
    std::set<int> idt;
    std::map<int, std::vector<int> > etaisyydet;
    // funktiot
    std::set<int> alueet() const {return(idt);}
    int matka(int alkuId, int loppuId);
    std::map<int, int> laske_dt(int id, std::vector<int> alut, std::vector<int> loput, int alueita);
};
std::set<int> Manner::alusta(int id, std::vector<int> kaikki_alut, std::vector<int> kaikki_loput) {
    std::set<int> tulos, uudet;
    tulos.insert(id);
    uudet.insert(id);
    while (uudet.size()>0) {
        std::set<int> apu;
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
    // Lasketaan keskinaiset etaisyydet
    // Tulee tehtyä kahteen kertaan, kun symmetriaa ei hyödynnetä :/
    std::map<int, std::vector<int> > lasketut_etaisyydet;
    std::map<int, int> yhden_etaisyydet;
    for (std::set<int>::iterator lahtop = tulos.begin(); lahtop != tulos.end(); ++lahtop) {
        yhden_etaisyydet = laske_dt(*lahtop, kaikki_alut, kaikki_loput, idt.size());
    }
t(yhden_etaisyydet);
//std::cerr << *lahtop << std::endl;
//    lasketut_etaisyydet[*lahtop] = yhden_etaisyydet;
    
    etaisyydet = lasketut_etaisyydet;
    std::cerr << "Etäisyydet laskettu, tässä eka rivi: " << std::endl;
        t(etaisyydet[*tulos.begin()]);
    std::cerr << std::endl;
    return (tulos);
}
int Manner::matka(int alkuId, int loppuId) {
    return((alkuId == loppuId)?0:etaisyydet[alkuId][loppuId]);
}
std::map<int, int> Manner::laske_dt(int id, std::vector<int> kaikki_alut, std::vector<int> kaikki_loput, int alueita) {
    std::map <int, int> yhden_etaisyydet;
    std::set<int> jo_loydetyt, edelliset, uusimmat;
    jo_loydetyt.insert(id);
    edelliset.insert(id);
    int et_nyt =1;
    while (jo_loydetyt.size() < alueita && et_nyt < 14) {
        for (std::set<int>::iterator valietappi = edelliset.begin(); valietappi != edelliset.end(); ++valietappi) {
            for (int tt = 0; tt < kaikki_alut.size(); ++tt) {
                if (kaikki_alut[tt] == *valietappi && jo_loydetyt.count(kaikki_loput[tt]) == 0) {
                    uusimmat.insert(kaikki_loput[tt]);
                    jo_loydetyt.insert(kaikki_loput[tt]);
                }
                if (kaikki_loput[tt] == *valietappi && jo_loydetyt.count(kaikki_alut[tt]) == 0) {
                    uusimmat.insert(kaikki_alut[tt]);
                    jo_loydetyt.insert(kaikki_alut[tt]);
                }
            }
        }
        for (std::set<int>::iterator uusi = uusimmat.begin(); uusi != uusimmat.end(); ++uusi) {
            yhden_etaisyydet[*uusi] = et_nyt;
// if (yhden_etaisyydet.size() < 1+ *uusi ) std::cerr << "Virhe!!!!!!!!" <<std::endl;
        }
        edelliset = uusimmat;
        uusimmat.clear();
        ++et_nyt;
        }
std::cerr << std::endl << "Löydetty " << jo_loydetyt.size() << ", viimeksi " << edelliset.size() << ", löytämättä jäi " << alueita - jo_loydetyt.size() <<std::endl;
t(uusimmat);
t(jo_loydetyt);
t(yhden_etaisyydet);
    return(yhden_etaisyydet);
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
    std::vector < int > idt;
    std::vector < int > platinatuotot;
    std::vector < int > loput;
    std::vector < int > alut;
    // Vuorottain vaihtuvat tiedot
    std::vector < int > omistajat;
    std::vector < int > vihut;
    std::vector < int > omat;
    int platinum;  // <- "todellinen" / 20, eli moneen yksikköön on varaa    
    int vro;
    // Talteen laskettavat tiedot
    std::vector <int> houkuttelevuudet;
    std::set<Manner> mantereet;
    // Funktioita datan pyörittely**yn
    std::vector<int> jrj(std::vector<int> ehdokkaat, std::vector<int> ehdokkaiden_arvot);
    std::vector<int> omistukset(std::vector<int> ehdokkaat, int kenen);
    std::vector<int> laske_houkuttelevuudet(std::vector<int> ehdokkaat);
    bool altisko(int id);
    bool valmis_mannerko(Manner manner);    
  public:
    // Konstruktorit
    Infot();
    Infot ( int pelaajia, int omaId,
            std::vector<int> peliruudut, std::vector<int> platinatuotot,
            std::vector<int> alkupisteet, std::vector<int> loppupisteet);
    // API-funktiot
    void kierroksen_alku(std::vector < int > omistajat, std::vector < int > vihut, std::vector < int > omat, int platinaa);
    bool eka_kierrosko();
    std::vector<int> parhaat_alueet(int kenen); // -1: tuottavat alueet, 0: alueet, joihin voi sijoittaa joukkoja
    std::vector<int> joukkoja(int kenen); // 0 omia, 1 muiden
    int varaa() {return(platinum);}
    int pelaajia() {return(playerCount);}
    int montako_omaa(int id) {return(omat[id]);}
    std::vector<int> suunnat(int id, int moneenko_suuntaan);
    int maksa(int paljonko, int mihin);
    std::vector<int> naapurit(int id);
    int satunnaissuunta(int id);
    int houkutus(int id) {return(houkuttelevuudet[id]);}
    void poista_valmiit_mantereet();
    bool uhattunako(int id);
};

Infot::Infot() {}
Infot::Infot(int pelaajia, int id, std::vector<int> it, std::vector<int> tuotot, std::vector<int> lop, std::vector<int> alk) 
:  vro(0),
  playerCount(pelaajia),
  myId(id),
  idt(it),
  platinatuotot(tuotot),
  loput(lop),
  alut(alk)
{
    idt = jrj(idt, platinatuotot);
    std::set<Manner> uusi_kokoelma;
    std::vector<int> apu;
    std::vector<int> jaljella(idt);
    while (jaljella.size() > 0) {
        apu.clear();
        Manner uusi;
        std::set<int> menneet = uusi.alusta(jaljella[0], alut, loput);
        uusi_kokoelma.insert(uusi);
        for (int tt = 0; tt < jaljella.size(); ++tt)
            if (menneet.count(jaljella[tt])==0)
                apu.push_back(jaljella[tt]);
        jaljella = apu;
    }
    mantereet = uusi_kokoelma;
    std::cerr << "Mantereita luotu " << uusi_kokoelma.size() << ", niiden koot: " <<std::endl;
    for (std::set<Manner>::iterator it=mantereet.begin(); it != mantereet.end(); it++)
        std::cerr << (*it).idt.size() << " ";
    std::cerr << std::endl;
}

void Infot::kierroksen_alku(std::vector < int > om, std::vector < int > vih, std::vector < int > o, int platinaa) {
    std::cerr << ++vro << ". vuoro alkaa." << std::endl;
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
std::vector<int> Infot::parhaat_alueet(int kenen) {
    if (kenen == 0)
        kenen = myId;
    std::vector<int> tulos = omistukset(idt, kenen);
    if (kenen == myId) { // Lisätään omiin vielä tyhjät
        std::vector<int> apu = omistukset(idt, -1);
        tulos.insert(tulos.end(), apu.begin(), apu.end());
    }
    if (kenen == -1) { // Poistetaan tyhjistä tuottamattomat
        std::vector<int> apu;
        for (int tt = 0; tt < tulos.size(); ++tt)
            if (platinatuotot[tulos[tt]]>1)
                apu.push_back(tulos[tt]);
        tulos = apu;
    }
    return(jrj(tulos, houkuttelevuudet));
}
std::vector<int> Infot::omistukset(std::vector<int> ehdokkaat, int kenen) {
    std::vector<int> tulos;
    for (int tt = 0; tt < ehdokkaat.size(); ++tt)
        if (omistajat[ehdokkaat[tt]] == kenen)
            tulos.push_back(ehdokkaat[tt]);
    return(tulos);
}
std::vector<int> Infot::laske_houkuttelevuudet(std::vector<int> ehd) {
    std::vector<int> tulos(platinatuotot.size());
    for (int tt = 0; tt < ehd.size(); ++tt) {
        tulos[ehd[tt]] = 200*platinatuotot[ehd[tt]]; // Indeksöinti olennainen - pitää tarkistaa laske_houkuttelevuudet samalla indeksöinnillä!
        if (omistajat[ehd[tt]] != myId)
            tulos[ehd[tt]] = tulos[ehd[tt]] * 2 + 300;
        if (omistajat[ehd[tt]] == myId) {
            tulos[ehd[tt]] = tulos[ehd[tt]] / 20;
            if (uhattunako(ehd[tt]))
                tulos[ehd[tt]] = tulos[ehd[tt]] * 25 + 150;
            if (altisko(ehd[tt]))
                tulos[ehd[tt]] = tulos[ehd[tt]] + 50;
        }
    }
    // Alueeseen yhteydessä olevien alueiden vaikutus
    int etaisyys = 2; // Kuinka kaukaa
    int osuus = 20; // Kuinka monta prosenttia naapureiden arvosta siirtyy eteenpäin.
    std::vector<int> lisat(platinatuotot.size());
    for (int d = 0; d < etaisyys; ++d) {
        for (int tt = 0; tt < idt.size(); ++tt) {
            std::vector<int> naap = naapurit(idt[tt]);
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
std::vector<int> Infot::jrj(std::vector<int> ehdokkaat, std::vector<int> arvot) {
    if (ehdokkaat.size() < 2) return(ehdokkaat);
    int mx = 0;
    std::vector<int> lisattavat, hannille, tulos;
    for (int tt =0; tt < ehdokkaat.size(); ++tt) {
        mx = std::max(mx, arvot[ehdokkaat[tt]]);
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
std::vector<int> Infot::joukkoja(int kenen) { // 0 omia, 1 muiden
    std::vector<int> tulos;
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
std::vector<int> Infot::suunnat(int id, int moneenko_suuntaan) {
    std::vector<int> tulos;
    std::vector<int> ehdokkaat = naapurit(id);
    ehdokkaat.push_back(id); // Myös tämänhetkinen ruutu (eli liikkumattomuus) on otettava huomioon vaihtoehdoissa.
    while (ehdokkaat.size() < moneenko_suuntaan)
        ehdokkaat.push_back(id);
    reverse(ehdokkaat.begin(), ehdokkaat.end()); // Suositaan paikallaan pysymistä ärsyttävyyden std::minimoimiseksi.
    ehdokkaat = jrj(ehdokkaat, houkuttelevuudet);
    for (int tt = 0; tt < moneenko_suuntaan; ++tt)
    {
        tulos.push_back(ehdokkaat[tt]);
        houkuttelevuudet[ehdokkaat[tt]] /= 2; // Jonoon lisätyn liikkeen johonkin ruutuun tulee heikentää sen houkuttelevuutta.
    }
    return(tulos);
}
std::vector<int> Infot::naapurit(int id) {
    std::vector<int> tulos;
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
    std::vector<int> naaps = naapurit(id);
    for (int tt = 0; tt < naaps.size(); ++tt)
        if (vihut[naaps[tt]]>omat[tt])
            tulos = true;            
    return(tulos);
}
int Infot::satunnaissuunta(int id) {
    std::vector<int> na = naapurit(id);
    if (na.size() == 0) return(-1);
    int mx = na.size();
    int r = rand() % mx;
    int tulos = na[r];
    return(tulos);
}
bool Infot::altisko(int id) {
    bool tulos = false;
    std::vector<int> naaps = naapurit(id);
    for (int tt = 0; tt < naaps.size(); ++tt) 
        if (omistajat[naaps[tt]] != myId) 
            tulos = true;
    return(tulos);
}
void Infot::poista_valmiit_mantereet() {
    for (std::set<Manner>::iterator it = mantereet.begin(); it != mantereet.end(); ++it) {
        if (valmis_mannerko(*it)) {
            std::set<int> poistettavat = (*it).alueet();
            std::vector<int> karsitut_idt, karsitut_alut, karsitut_loput;
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
    std::set<int> alueet = manner.alueet();
    std::set<int>::iterator it = alueet.begin();
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
    std::vector<int> valmistele_liikkeet();
    void tee_liikkeet(std::vector<int> liik);
    std::vector<int> valmistele_ostot();
    void tee_ostot(std::vector<int> ost);
    void ekat_ostot(std::vector<int> & ost);
    void tyhjien_taytto(std::vector<int> & ost);
    void rautaa_rajalle(std::vector<int> & ost);
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
std::vector<int> Mekaniikka::valmistele_liikkeet() {
    std::vector<int> tulos;
    std::vector <int> alueet = info.joukkoja(0); // Omia joukkoja sisältävät ruudut
    for (int tt = 0; tt < alueet.size(); ++tt) {
        // Yli neljän ryppäitä kannattaa hajottaa
        int joukkokoko = (info.uhattunako(alueet[tt])) ? 4:1;
        int moneenko_suuntaan = info.montako_omaa(alueet[tt]) / joukkokoko + 1;
        std::vector<int> suun = info.suunnat(alueet[tt], moneenko_suuntaan);
        for (int ss = 0; ss < moneenko_suuntaan-1; ++ss) 
            if (suun[ss] != alueet[tt]) {
                tulos.push_back(joukkokoko);
                tulos.push_back(alueet[tt]);
                tulos.push_back(suun[ss]);
        }
        if (suun[moneenko_suuntaan-1] != alueet[tt]) {
            int jaaneet = info.montako_omaa(alueet[tt]) % joukkokoko;
            if (jaaneet) {
                tulos.push_back(jaaneet);
                tulos.push_back(alueet[tt]);
                tulos.push_back(suun[moneenko_suuntaan-1]);
            }
        }
    }
    return(tulos);
}
void Mekaniikka::tee_liikkeet(std::vector<int> liik) {
        std::cerr << "Liikkeitä: " << liik.size()/3 << std::endl;
    if (liik.size() < 3) {
        std::cout << "WAIT";
    } else {
        for (int tt = 0; tt < liik.size(); ++tt) 
            std::cout << liik[tt] << " ";
    }
    std::cout << std::endl;
}
std::vector<int> Mekaniikka::valmistele_ostot() {
    std::vector<int> tulos;
    if (info.eka_kierrosko()) {
        ekat_ostot(tulos);
    }
    if (info.varaa())
        tyhjien_taytto(tulos);
    if (info.varaa())
        rautaa_rajalle(tulos);
    return(tulos);
}
void Mekaniikka::tyhjien_taytto(std::vector<int> & ost) {
    std::vector<int> tyhj = info.parhaat_alueet(-1);
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
void Mekaniikka::rautaa_rajalle(std::vector<int> & ost) {
    std::vector<int> raja = info.parhaat_alueet(0);
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
void Mekaniikka::tee_ostot(std::vector<int> ost) {
        std::cerr << "Ostoja: " << ost.size()/2 << std::endl;
    if (ost.size() < 2) {
        std::cout << "WAIT";
    } else {
        for (int tt = 0; tt < ost.size(); ++tt)
            std::cout << ost[tt] << " ";
    }
    std::cout << std::endl;
}
void Mekaniikka::ekat_ostot(std::vector<int> & ost) {
    std::vector<int> tyhj = info.parhaat_alueet(-1);
    int tt = 0;
    int r = 1 + rand() % 4;
    while (tt < tyhj.size() && tt < r && info.varaa() > 0) { // Houkuttelevimpaan laitetaan 2+, sillä näin voitetaan yleisin kanssapelaajien strategia laittaa kaikkiin houkutteleviin 1...
        int parhaaseen = rand() % 3;
        parhaaseen = parhaaseen -1 + info.pelaajia();
        parhaaseen = std::min(4, parhaaseen);
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
      std::cin >> playerCount >> myId >> zoneCount >> linkCount;
      std::cin.ignore();
      
      
      // Omat muuttujat
      std::vector < int > idt(zoneCount);
      std::vector < int > platinatuotot(zoneCount);
      std::vector < int > loput(linkCount);
      std::vector < int > alut(linkCount);
      std::vector < int > omistajat(zoneCount);
      std::vector < int > vihut(zoneCount);
      std::vector < int > omat(zoneCount);

      for (int i = 0; i < zoneCount; i++) {
         //        int zoneId; // this zone's ID (between 0 and zoneCount-1)
         //        int platinumSource; // the amount of Platinum this zone can provide per game turn
         std::cin >> idt[i] >> platinatuotot[i];
         std::cin.ignore();
      }
      for (int i = 0; i < linkCount; i++) {
         std::cin >> alut[i] >> loput[i];
         std::cin.ignore();
      }

      Infot infot(playerCount, myId, idt, platinatuotot, alut, loput);
      
      // game loop
    while (1) {
        int platinum; // my available Platinum
        std::cin >> platinum;
        std::cin.ignore();
        for (int i = 0; i < zoneCount; i++) {
            int zId; // this zone's ID
            int ownerId; // the player who owns this zone (-1 otherwise)
            int podsP0; // player 0's PODs on this zone
            int podsP1; // player 1's PODs on this zone
            int podsP2; // player 2's PODs on this zone (always 0 for a two player game)
            int podsP3; // player 3's PODs on this zone (always 0 for a two or three player game)
            std::cin >> zId >> ownerId >> podsP0 >> podsP1 >> podsP2 >> podsP3;
            std::cin.ignore();
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
