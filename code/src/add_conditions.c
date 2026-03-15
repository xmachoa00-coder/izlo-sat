#include "add_conditions.h"
#include <stddef.h>
#include <stdio.h>
#include "cnf.h"
#include "parse.h"

//
// LOGIN: xmachoa00
//

/** Funkce demonstrující vytvoření nové (arbitrárně vybrané) klauzule
* ve tvaru "A_{0,1,2} || -B_{0, 1, 2}" do výrokové formule
* @param formula výroková formule, do níž bude klauzule přidána
*/
void conditions_example(CNF* formula) {
    assert(formula != NULL);

    // vytvoření nové klauzule
    Clause* cl = create_new_clause(formula);

    // přidání proměnné A_{0,1,2} do klauzule
    // proměnná říká, že ulice spojující rozcestí 0 a 1 bude opravována v první fázi v den s indexem 2
    // cl - klauzule, do níž přidáváme literál
    // true - značí, že přidaný literál je pozitivní proměnná
    // FITST_PHASE_FLAG - značí, že aktuální proměnná je pro první fázi opravy
    // 0 - značí výchozí rozcestí s indexem 0
    // 1 - značí cílové rozcestí s indexem 1
    // 2 - značí den s indexem 2
    add_literal_to_clause(cl, true, FIRST_PHASE_FLAG, 0, 1, 2);

    // přidání proměnné -B_{0,1,2} do klauzule
    // proměnná říká, že ulice spojující rozcestí 0 a 1 nebude opravována v druhé fázi v den s indexem 2
    // cl - klauzule, do níž přidáváme literál
    // false - značí, že přidaný literál je negativní proměnná
    // SECOND_PHASE_FLAG - značí, že aktuální proměnná je pro druhou fázi opravy
    // 0 - značí výchozí rozcestí s indexem 0
    // 1 - značí cílové rozcestí s indexem 1
    // 2 - značí den s indexem 2
    add_literal_to_clause(cl, false, SECOND_PHASE_FLAG, 0, 1, 2);
}

/** Funkce vytvářející klauzule ošetřující podmínku 1 ze zadání
* @param formula výroková formule, do níž bude klauzule přidána
* @param num_of_days počet dní
* @param num_of_crossroads počet rozcestí
* @param num_of_streets počet ulic
* @param neighbours seznamy sousedů
* @param streets seznam ulic
*/
void all_streets_min_one_day_of_first_phase_roadwork(CNF *formula, unsigned num_of_days, unsigned num_of_crossroads, unsigned num_of_streets, const NeighbourLists *neighbours, const Street *streets) {

    assert(formula != NULL);
    assert(num_of_crossroads >= 2);
    assert(num_of_days > 0);

    // Každá klauzule bude vyjadřovat podmínku, že daná silnice je opravována
    // v první fázi alespoň v jednom z dostupných dnů
    // Existuje-li například silnice (0, 1) mezi rozcestími 0 a 1, pak pro
    // n dní bude mít klauzule tvar:
    // A_{0, 1, 0} || A_{0, 1, 1} || A_{0, 1, 2} || ... || A_{0, 1, n-1},
    // což ve formátu DIMACS odpovídá klauzuli:
    // A_{0, 1, 0} A_{0, 1, 1} A_{0, 1, 2} ... A_{0, 1, n-1} 0
    // (přičemž každá z proměnných A_{0, 1, d} je zakódována jako jedno celé číslo)
    for(unsigned street_idx = 0; street_idx < num_of_streets; ++street_idx) {
        Street street = streets[street_idx];
        Clause *cl = create_new_clause(formula);

        // Pro každou ulici vyjádříme podmínku, že musí být opravena v první fázi v alespoň jednom dni

        for(unsigned day = 0; day < num_of_days; ++day) {
            add_literal_to_clause(cl, true, FIRST_PHASE_FLAG, street.source, street.destination, day);
        }
    }

    /* Alternativní varianta:

    // iterujeme přes veškerá výstupní rozcestí
    for(unsigned src = 0; src < num_of_crossroads; ++src) {

        // iterujeme přes veškerá vstupní rozcestí, přičemž dle zadání vždy platí dst > src
        for(unsigned dst = src + 1; dst < num_of_crossroads; ++dst) {

            // zajímají nás pouze ty dvojice rozcestí, mezi nimiž skutečně existuje silnice
            if(are_neighbours(neighbours, src, dst)) {

                // dále budeme iterovat přes dny, tudíž zde vytvoříme čerstvou klauzuli
                Clause *cl = create_new_clause(formula);

                // iterujeme přes dny, abychom mohli přidávat odpovídající proměnné
                // do nově vytvořené klauzule
                for(unsigned day = 0; day < num_of_days; ++day) {
                    // přidání proměnné x_{src, dst, day} do klauzule cl
                    // příznaky vyjadřují, že jde o proměnnou v pozitivním tvaru a že odpovídá první fázi oprav
                    add_literal_to_clause(cl, true, FIRST_PHASE_FLAG, src, dst, day);
                }
            }
        }
    }*/
}

/** Funkce vytvářející klauzule ošetřující podmínku 2 ze zadání
* @param formula výroková formule, do níž bude klauzule přidána
* @param num_of_days počet dní
* @param num_of_crossroads počet rozcestí
* @param num_of_streets počet ulic
* @param neighbours seznamy sousedů
* @param streets seznam ulic
*/
void all_streets_max_one_day_of_first_phase_roadwork(CNF *formula, unsigned num_of_days, unsigned num_of_crossroads, unsigned num_of_streets, const NeighbourLists *neighbours, const Street *streets) {
 //(!Fáze1_den1 or !Fáze1_den2)
    for(int index_street = 0; index_street < num_of_streets; index_street++){
        Street st = streets[index_street];
        for(int day1 = 0; day1 < num_of_days; day1++){
            for(int day2 = day1 + 1; day2 < num_of_days; day2++){

                Clause *c = create_new_clause(formula);
                add_literal_to_clause(c, false, FIRST_PHASE_FLAG, st.source, st.destination, day1);

                add_literal_to_clause(c, false, FIRST_PHASE_FLAG, st.source, st.destination, day2);
            }
        }
    }
}




/** Funkce vytvářející klauzule ošetřující podmínku 3 ze zadání
* @param formula výroková formule, do níž bude klauzule přidána
* @param num_of_days počet dní
* @param num_of_crossroads počet rozcestí
* @param num_of_streets počet ulic
* @param neighbours seznamy sousedů
* @param streets seznam ulic
*/
void second_phase_follows_first_immediately(CNF *formula, unsigned num_of_days, unsigned num_of_crossroads, unsigned num_of_streets, const NeighbourLists *neighbours, const Street *streets) {
    //(!Fáze1_den or Fáze2_den+1), (!Fáze2_den or Fáze1_den-1), A<=>B
    for (int index_street = 0; index_street < num_of_streets; index_street++){
        Street st = streets[index_street];
        for (int day = 0; day < num_of_days; day++){

            Clause *c1 = create_new_clause(formula);
            add_literal_to_clause(c1, false, FIRST_PHASE_FLAG, st.source, st.destination,day);

            if(!(day == num_of_days - 1)){
                add_literal_to_clause(c1, true, SECOND_PHASE_FLAG, st.source, st.destination,day + 1);
            }

            Clause *c2 = create_new_clause(formula);//zabrání aby 2 Fáze proběhla bez první
            add_literal_to_clause(c2, false, SECOND_PHASE_FLAG, st.source, st.destination, day);
            if (day > 0) {
                add_literal_to_clause(c2, true, FIRST_PHASE_FLAG, st.source, st.destination, day - 1);
            }
        }
    }
}

/** Funkce vytvářející klauzule ošetřující podmínku 4 ze zadání
* @param formula výroková formule, do níž bude klauzule přidána
* @param num_of_days počet dní
* @param num_of_crossroads počet rozcestí
* @param num_of_streets počet ulic
* @param neighbours seznamy sousedů
* @param streets seznam ulic
*/
void neighbour_streets_not_being_repaired_simultaneously(CNF *formula, unsigned num_of_days, unsigned num_of_crossroads, unsigned num_of_streets, const NeighbourLists *neighbours, const Street *streets) {
    //neg((Fáze1_street1_den or Fáze2_street1_den) and (Fáze1_street2_den or Fáze2_street2_den)), toto upravíme pomocí De Morganových zákonů a distributivity
    for (unsigned index_street1 = 0; index_street1 < num_of_streets; index_street1++) {
        for (unsigned index_street2 = index_street1 + 1; index_street2 < num_of_streets; index_street2++) {
            Street s1 = streets[index_street1];
            Street s2 = streets[index_street2];

            if ((s1.source == s2.source )|| (s1.source == s2.destination) || (s1.destination == s2.source) || (s1.destination == s2.destination)) {
                //zkoumáme, jestli mají společnou křižovatku
                for (unsigned day = 0; day < num_of_days; day++) {
                    Clause *c1 = create_new_clause(formula);
                    add_literal_to_clause(c1, false, FIRST_PHASE_FLAG, s1.source, s1.destination, day);
                    add_literal_to_clause(c1, false, FIRST_PHASE_FLAG, s2.source, s2.destination, day);

                    Clause *c2 = create_new_clause(formula);
                    add_literal_to_clause(c2, false, FIRST_PHASE_FLAG, s1.source, s1.destination, day);
                    add_literal_to_clause(c2, false, SECOND_PHASE_FLAG, s2.source, s2.destination, day);

                    Clause *c3 = create_new_clause(formula);
                    add_literal_to_clause(c3, false, SECOND_PHASE_FLAG, s1.source, s1.destination, day);
                    add_literal_to_clause(c3, false, FIRST_PHASE_FLAG, s2.source, s2.destination, day);

                    Clause *c4 = create_new_clause(formula);
                    add_literal_to_clause(c4, false, SECOND_PHASE_FLAG, s1.source, s1.destination, day);
                    add_literal_to_clause(c4, false, SECOND_PHASE_FLAG, s2.source, s2.destination, day);
                }
            }
        }
    }
}


/** Funkce vytvářející klauzule ošetřující podmínku 5 ze zadání
* @param formula výroková formule, do níž bude klauzule přidána
* @param num_of_days počet dní
* @param num_of_crossroads počet rozcestí
* @param num_of_streets počet ulic
* @param neighbours seznamy sousedů
* @param streets seznam ulic
*/
void each_day_at_least_one_street_being_repaired(CNF *formula, unsigned num_of_days, unsigned num_of_crossroads, unsigned num_of_streets, const NeighbourLists *neighbours, const Street *streets) {
    //(Fáze1_street1 or Fáze2_street1 or Fáze1_street2 or Fáze2_street2 ...), jedna velká klauzule pro každý den, aspoň jeden literál v ní musí být pravdivý
    for (int day = 0; day < num_of_days; day++){
        Clause *c = create_new_clause(formula);//pouze jedna klauzule pro jeden den
        for(int index_street = 0; index_street < num_of_streets; index_street++){
            Street st = streets[index_street];
            add_literal_to_clause(c, true, FIRST_PHASE_FLAG, st.source, st.destination, day);
            add_literal_to_clause(c, true, SECOND_PHASE_FLAG, st.source, st.destination, day);
        }
    }

}

/** Funkce vytvářející klauzule ošetřující podmínku 6 ze zadání
* @param formula výroková formule, do níž bude klauzule přidána
* @param num_of_days počet dní
* @param num_of_crossroads počet rozcestí
* @param num_of_streets počet ulic
* @param neighbours seznamy sousedů
* @param streets seznam ulic
*/
void street_between_0_and_1_repaired_in_last_two_days(CNF *formula, unsigned num_of_days, unsigned num_of_crossroads, unsigned num_of_streets, const NeighbourLists *neighbours, const Street *streets) {
    //dvě samostatné klauzule pouze s jedním literálem, solver nemá jinou možnost než je splnit
    if (are_neighbours(neighbours, 0, 1)){
        if(num_of_days >= 2){
            Clause *c1 = create_new_clause(formula);
            add_literal_to_clause(c1, true, FIRST_PHASE_FLAG, 0, 1, num_of_days -2);//předposlední den

            Clause *c2 = create_new_clause(formula);
            add_literal_to_clause(c2, true, SECOND_PHASE_FLAG, 0, 1, num_of_days -1);//poslední den
        }
    }
  
}

/** Funkce vytvářející klauzule ošetřující podmínku 7 ze zadání
* @param formula výroková formule, do níž bude klauzule přidána
* @param num_of_days počet dní
* @param num_of_crossroads počet rozcestí
* @param num_of_streets počet ulic
* @param neighbours seznamy sousedů
* @param streets seznam ulic
*/
void no_street_to_0_repaired_during_weekend(CNF *formula, unsigned num_of_days, unsigned num_of_crossroads, unsigned num_of_streets, const NeighbourLists *neighbours, const Street *streets) {
    //pokud je víkend => NENÍ PRAVDA (Fáze1 or Fáze2) na ulicích u křižovatky 0
    for(int day = 0; day < num_of_days; day++){
        if ((day % 7 == 5)||(day % 7 == 6)){//podmínka pouze pro víkendy
            for(int index_street = 0; index_street < num_of_streets; index_street++){
                Street st = streets[index_street];
                if((st.source == 0)||(st.destination == 0)){ //pokud sousedí s letištem
                    Clause *c1 = create_new_clause(formula);
                    add_literal_to_clause(c1, false, FIRST_PHASE_FLAG, st.source, st.destination,day);
                    Clause *c2 = create_new_clause(formula);
                    add_literal_to_clause(c2, false, SECOND_PHASE_FLAG, st.source, st.destination,day);
                }
            }
        }
    }
}

