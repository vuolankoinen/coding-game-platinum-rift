#include <iostream> 
#include <vector> 
#include <set> 
#include <map> 
#include <math.h> 
#include <algorithm>

/*                                      
Note that the code is all in one file, because it needs to be written in a single window in the codingame-IDE.
*/

int prospotenssiin(int x, int pr, int pow) {
    if (pow > 6 || x == 0) return(0);
    double tulos = (double)x;
    for (int tt = 0; tt < pow; ++tt)
        tulos = tulos * pr / 100.0;
    return(std::floor(tulos));
}

/***********  MANNER-luokka ***********/
/*************************************/
// Toisiinsa yhteydessä olevien ruutujen joukko
struct Manner {
    // Pseudo-konstruktori, palauttaa mantereen alueet
    std::set<int> alusta(int id, std::vector<int> alut, std::vector<int> loput, std::vector<int> tuotot);
    // arvot
    std::set<int> idt;
    std::map<std::pair<int, int>, int > etaisyydet;
    int platinaa; // Ennen tätä sijoitus oli 140, jälkeen 112
    // funktiot
    std::set<int> alueet() const {return(idt);}
    int matka(int alkuId, int loppuId);
    void laske_dt(std::map<std::pair<int, int>, int> & etaisyydet, int id, std::vector<int> alut, std::vector<int> loput, int alueita);
};
std::set<int> Manner::alusta(int id, std::vector<int> kaikki_alut, std::vector<int> kaikki_loput, std::vector<int> tuotot) {
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
    std::map<std::pair<int, int>, int> etais;
    for (std::set<int>::iterator lahtop = tulos.begin(); lahtop != tulos.end(); ++lahtop)
        laske_dt(etais, *lahtop, kaikki_alut, kaikki_loput, idt.size());
    etaisyydet = etais;
    // Lasketaan mantereen platinatuotot.
    platinaa = 0;
    for (std::set<int>::iterator it=tulos.begin(); it!=tulos.end(); ++it)
        platinaa = platinaa + tuotot[*it];
    return (tulos);
}
int Manner::matka(int alkuId, int loppuId) {
    if (idt.count(alkuId) == 0 || idt.count(loppuId) == 0) {
        return(10000); // Ainakin toinen päätepiste on toisella mantereella.
        std::cerr << "HEP! Pyydetty väärän mantereen etäisyyksiä... " << alkuId << " ja " << loppuId << std::endl;
    }
    return(etaisyydet[std::pair<int, int> (alkuId,loppuId)]);
}
void  Manner::laske_dt(std::map<std::pair<int, int>, int> & etaisyydet, int id, std::vector<int> kaikki_alut, std::vector<int> kaikki_loput, int alueita) {
    std::set<int> jo_loydetyt, edelliset, uusimmat;
    jo_loydetyt.insert(id);
    edelliset.insert(id);
    etaisyydet[std::pair<int, int> (id, id)] = 0;
    int et_nyt = 1;
    while (edelliset.size() > 0) {
//    while (jo_loydetyt.size() < alueita) {
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
            etaisyydet[std::pair<int, int>(*uusi, id)] = et_nyt;
            etaisyydet[std::pair<int, int>(id, *uusi)] = et_nyt;
        }
        edelliset = uusimmat;
        uusimmat.clear();
        ++et_nyt;
        if (et_nyt > 40) {
            std::cerr << "Etäisyyksiä ei saatu laskettua!" << std::endl;
            return;
        }
        }
}


bool operator < ( Manner const &o, Manner const &v) {
    if (v.idt.size() < o.idt.size())
        return(true);
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
    int aikaa_viepia_tehty;
    // Funktioita datan pyörittely**yn
    std::vector<int> jrj(std::vector<int> ehdokkaat, std::vector<int> ehdokkaiden_arvot);
    std::vector<int> omistukset(std::vector<int> ehdokkaat, int kenen);
    std::vector<int> laske_pohjahoukuttelevuudet(std::vector<int> ehdokkaat);
    std::vector<int> laske_verkostohoukuttelevuudet(Manner manner, std::vector<int> jo_olemassa, std::vector<int> pohjahoukuttelevuudet);
    bool altisko(int id);
    bool valmis_mannerko(Manner manner);    
    int pelaajia() {return(playerCount);}
    std::vector<int> reitti_omalta_alueelta(int id);
    void lisaa_ohjaava_gradientti(int id);
    int etsi_lahin_vihu(int id);
    int etsi_lahin_naapuri(int alkaen_mista, int kenen);
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
    int montako_omaa(int id) {return(omat[id]);}
    std::vector<int> suunnat(int id, int moneenko_suuntaan);
    int maksa(int paljonko, int mihin);
    std::vector<int> naapurit(int id);
    int houkutus(int id) {return(houkuttelevuudet[id]);}
    void poista_valmiit_mantereet();
    bool uhattunako(int id);
    void lisaa_mannerten_painoarvot();
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
        std::set<int> menneet = uusi.alusta(jaljella[0], alut, loput, platinatuotot);
        uusi_kokoelma.insert(uusi);
        for (int tt = 0; tt < jaljella.size(); ++tt)
            if (menneet.count(jaljella[tt])==0)
                apu.push_back(jaljella[tt]);
        jaljella = apu;
    }
    mantereet = uusi_kokoelma;
}

void Infot::kierroksen_alku(std::vector<int> om, std::vector<int> vih, std::vector<int> o, int platinaa) {
    ++vro; // Tämä oli tippunut - ilman tätä 112, tämän kanssa 1
    aikaa_viepia_tehty = 0;
    omistajat = om;
    vihut = vih;
    omat = o;
    platinum = platinaa;
    std::vector<int> pohjaht = laske_pohjahoukuttelevuudet(idt);
    std::vector<int> jo(platinatuotot.size(), 0);
    poista_valmiit_mantereet();
    for (std::set<Manner>::iterator it = mantereet.begin(); it != mantereet.end(); ++it) {
        jo = laske_verkostohoukuttelevuudet(*it, jo, pohjaht);
    }
    houkuttelevuudet = jo;
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
std::vector<int> Infot::laske_pohjahoukuttelevuudet(std::vector<int> ehd) {
    int alttiushaitta = 60;  // 25: 108/124, 35: 121/124, 50: 105/128, 150: 123/124
    std::vector<int> tulos(platinatuotot.size());
    for (int tt = 0; tt < ehd.size(); ++tt) {
        if (omistajat[ehd[tt]] != myId) {
            if (omistajat[ehd[tt]] == -1) {
                tulos[ehd[tt]] = 200 * platinatuotot[ehd[tt]] + alttiushaitta * (playerCount-1);
            } else {
                tulos[ehd[tt]] = 120 * platinatuotot[ehd[tt]] + alttiushaitta;
                if (!uhattunako(ehd[tt])) {
                    tulos[ehd[tt]] = 2 * tulos[ehd[tt]];
                }
                if (vihut[ehd[tt]]==0) {
                    tulos[ehd[tt]] = 2 * tulos[ehd[tt]];
                }
            }
        }
        if (omistajat[ehd[tt]] == myId) {
            tulos[ehd[tt]] = 20 * platinatuotot[ehd[tt]];
            if (uhattunako(ehd[tt])) {
                tulos[ehd[tt]] = tulos[ehd[tt]] * 4 + 100;
            }
            tulos[ehd[tt]] = tulos[ehd[tt]] + alttiushaitta;
            if (!altisko(ehd[tt])) {
                tulos[ehd[tt]] = 0;
            }
        }
    }
    return(tulos);
}
// Lisätään mantereittain tuottojen perusteella tasuriratkaisijoita.
void Infot::lisaa_mannerten_painoarvot() {
    for (std::set<Manner>::iterator it = mantereet.begin(); it != mantereet.end(); ++it) {
        if (!valmis_mannerko(*it)) {
            std::set<int> al = (*it).alueet();
            for (std::set<int>::iterator al_it = al.begin(); al_it != al.end(); ++al_it)
                houkuttelevuudet[*al_it] += (*it).platinaa;
        }
    }
}
std::vector<int> Infot::laske_verkostohoukuttelevuudet(Manner manner, std::vector<int> jo, std::vector<int> pohjaht) {
    // Alueeseen yhteydessä olevien alueiden vaikutus
    if (valmis_mannerko(manner)) return(jo);
    int osuus = 20; // Kuinka monta prosenttia naapureiden arvosta siirtyy eteenpäin.
    std::set<int> alueet = manner.alueet();
    for (std::set<int>::iterator it = alueet.begin(); it != alueet.end(); ++it) {
        int houk = pohjaht[*it];
        for (std::set<int>::iterator muut_it = alueet.begin(); muut_it != alueet.end(); ++muut_it) {
            jo[*muut_it] += prospotenssiin(houk, osuus, manner.matka(*it, *muut_it));
        }
    }
    return(jo);
}

int Infot::maksa(int x, int id) {
    platinum -= x;
    houkuttelevuudet[id] = houkuttelevuudet[id]*2/3;
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
    reverse(ehdokkaat.begin(), ehdokkaat.end()); // Suositaan paikallaan pysymistä ärsyttävyyden minimoimiseksi.
    ehdokkaat = jrj(ehdokkaat, houkuttelevuudet);
    while (ehdokkaat.size()>0 && houkuttelevuudet[ehdokkaat[ehdokkaat.size()-1]]==0)
        ehdokkaat.pop_back();
    if (ehdokkaat.size() == 0) 
        return(reitti_omalta_alueelta(id));
    for (int tt = 0; tt < moneenko_suuntaan; ++tt) {
        tulos.push_back(ehdokkaat[tt]);
        if (vihut[ehdokkaat[tt]]==0) houkuttelevuudet[ehdokkaat[tt]] /= 2; // Jonoon lisätyn liikkeen johonkin ruutuun tulee heikentää sen houkuttelevuutta.
    }
    return(tulos);
}
std::vector<int> Infot::reitti_omalta_alueelta(int id) {
    std::vector<int> uus;
    std::vector<int> ehdokkaat = naapurit(id);
    if (++aikaa_viepia_tehty > 5) {
        uus.push_back(ehdokkaat[0]);
        return(uus);
    }
    int tulos(id);
    int kohde = etsi_lahin_vihu(id);
    bool kesken = true;
    while (kesken) {
        // Tarkistetaan, tuliko valmista
        for (std::vector<int>::iterator it = ehdokkaat.begin(); it != ehdokkaat.end(); ++it) {
            if (kohde == *it) {
                tulos = *it;
                kesken = false;
            }
        }
        // Etsitään seuraavaksi lähin kohde
        kohde = etsi_lahin_naapuri(id, kohde);
    }
    uus.push_back(tulos);
    return(uus);
}
int Infot::etsi_lahin_vihu(int id) {
    int tulos(id);
    std::set<int> jo_loydetyt, edelliset, uusimmat;
    jo_loydetyt.insert(id);
    edelliset.insert(id);
    while (edelliset.size() > 0 && tulos == id) {
        for (std::set<int>::iterator valietappi = edelliset.begin(); valietappi != edelliset.end(); ++valietappi) {
            for (int ss = 0; ss < alut.size(); ++ss) {
                if (alut[ss] == *valietappi && jo_loydetyt.count(loput[ss]) == 0)
                    uusimmat.insert(loput[ss]);
                if (loput[ss] == *valietappi && jo_loydetyt.count(alut[ss]) == 0)
                    uusimmat.insert(alut[ss]);
            }
        }
        for (std::set<int>::iterator uusi = uusimmat.begin(); uusi != uusimmat.end(); ++uusi) {
            if (omistajat[*uusi] != myId || vihut[*uusi]>0)
                tulos = *uusi;
        }
        edelliset = uusimmat;
        uusimmat.clear();
    }
    if (tulos == id)
        std::cerr << "Varoitus: etsittyä vihua ei löytynyt." << std::endl;
    return(tulos);
}
int Infot::etsi_lahin_naapuri(int alkaen_mista, int id) {
    std::vector<int> kohteet = naapurit(id);
    int tulos(alkaen_mista);
    std::set<int> jo_loydetyt, edelliset, uusimmat;
    jo_loydetyt.insert(alkaen_mista);
    edelliset.insert(alkaen_mista);
    while (edelliset.size() > 0 && tulos == alkaen_mista) {
        for (std::set<int>::iterator valietappi = edelliset.begin(); valietappi != edelliset.end(); ++valietappi) {
            for (int ss = 0; ss < alut.size(); ++ss) {
                if (alut[ss] == *valietappi && jo_loydetyt.count(loput[ss]) == 0)
                    uusimmat.insert(loput[ss]);
                if (loput[ss] == *valietappi && jo_loydetyt.count(alut[ss]) == 0)
                    uusimmat.insert(alut[ss]);
            }
        }
        for (std::set<int>::iterator uusi = uusimmat.begin(); uusi != uusimmat.end(); ++uusi)
            for (int kk = 0; kk < kohteet.size(); ++kk)
                if (kohteet[kk] == *uusi)
                    tulos = *uusi;
        edelliset = uusimmat;
        uusimmat.clear();
    }
    if (tulos == alkaen_mista)
        std::cerr << "Varoitus: etsittyä vihua ei löytynyt." << std::endl;
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
    if (vihut[id] > 0)
        tulos = true;
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
            // Tallennetaan pelitilanteeseen (info-olio) typistetyt tiedot.
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

/*************** vektoripari **************/
/******************************************/
struct Vektoripari {
    Vektoripari() {}
    Vektoripari(std::vector<int> e, std::vector<int> t) : lkmt(e), indeksit(t) {}    
    std::vector<int> lkmt;
    std::vector<int> indeksit;
    void lisaa_idlle(int id, int paljonko = 1);
};
void Vektoripari::lisaa_idlle(int id, int paljonko ) {
    int tt =0;
    bool kesken = true;
    while (kesken && tt < lkmt.size()) {
        if (indeksit[tt] == id) {
            ++lkmt[tt];
            kesken = false;
        }
        ++tt;
    }
    if (kesken) {
        lkmt.push_back(1);
        indeksit.push_back(id);
    }
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
    Vektoripari valmistele_ostot();
    void tee_ostot(Vektoripari ost_kohteet);
    void ekat_ostot(Vektoripari & ost);
    void tyhjien_taytto(Vektoripari & ost);
    void rautaa_rajalle(Vektoripari & ost);
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
        moneenko_suuntaan = suun.size();
        for (int ss = 0; ss < moneenko_suuntaan-1; ++ss) 
            if (suun[ss] != alueet[tt]) {
                tulos.push_back(joukkokoko);
                tulos.push_back(alueet[tt]);
                tulos.push_back(suun[ss]);
        }
        // Jakojäännös vielä.
        if (moneenko_suuntaan > 0 && suun[moneenko_suuntaan-1] != alueet[tt]) {
            int jaaneet = (joukkokoko == 1)? info.montako_omaa(alueet[tt]) - joukkokoko * (moneenko_suuntaan-1) : info.montako_omaa(alueet[tt]) % joukkokoko;
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
    if (liik.size() < 3) {
        std::cout << "WAIT";
    } else {
        for (int tt = 0; tt < liik.size(); ++tt) 
            std::cout << liik[tt] << " ";
    }
    std::cout << std::endl;
}
Vektoripari Mekaniikka::valmistele_ostot() {
    Vektoripari tulos;
    info.lisaa_mannerten_painoarvot();
    if (info.eka_kierrosko()) 
        ekat_ostot(tulos);
    if (info.varaa())
        tyhjien_taytto(tulos);
    while (info.varaa())
        rautaa_rajalle(tulos);
    return(tulos);
}
void Mekaniikka::tyhjien_taytto(Vektoripari & ost) {
    std::vector<int> tyhj = info.parhaat_alueet(-1);
    if (tyhj.size() >0 && info.varaa() > 0) {
        int apu = 0;
        while (info.varaa() > 0 && apu < tyhj.size()) {
            ost.lisaa_idlle(tyhj[apu]);
            info.maksa(1, tyhj[apu]);
            ++apu;
        }
    }
}
void Mekaniikka::rautaa_rajalle(Vektoripari & ost) {
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
        ost.lisaa_idlle(raja[apu], kuhunkin);
        info.maksa(1, raja[apu]);
        ++apu;
    }
}
void Mekaniikka::tee_ostot(Vektoripari ost) {
    std::vector<int> ost_maarat = ost.lkmt;
    std::vector<int> ost_kohteet = ost.indeksit;
    if (ost_kohteet.size() != ost_maarat.size()) {
        std::cout << "WAIT";
        std::cerr << "Virheelliset ostot!" << std::endl;
        return;
    }
    if (ost_kohteet.size() < 2) {
        std::cout << "WAIT";
    } else {
        for (int tt = 0; tt < ost_kohteet.size(); ++tt) 
            std::cout << ost_maarat[tt] << " " << ost_kohteet[tt] << " ";
    }
    std::cout << std::endl;
}
void Mekaniikka::ekat_ostot(Vektoripari & ost) {
    std::vector<int> tyhj = info.parhaat_alueet(-1);
    int tt = 0;
    int r = 1 + rand() % 4;
    while (tt < tyhj.size() && tt < r && info.varaa() > 0) { // Houkuttelevimpaan laitetaan 2+, sillä näin voitetaan yleisin kanssapelaajien strategia laittaa kaikkiin houkutteleviin 1...
        int parhaaseen = rand() % 4; // 3: 122, 4: 107
        parhaaseen = parhaaseen -1;// + info.pelaajia(); // Tämän kanssa 318 / 762, poiston jälkeen 212 / 762!
        parhaaseen = std::min(4, parhaaseen);
        ost.lisaa_idlle(tyhj[tt], (parhaaseen <= info.varaa()) ? parhaaseen:info.varaa() );
        info.maksa((parhaaseen <= info.varaa()) ? parhaaseen:info.varaa(), tyhj[tt]);
        ++tt;
    }
    while (tt < tyhj.size() && info.varaa() > 0) {
        ost.lisaa_idlle(tyhj[tt]);
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