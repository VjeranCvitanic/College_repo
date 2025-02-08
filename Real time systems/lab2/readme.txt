Glavna dretva stvara N dretvi simulatora, gdje je N dobiven na osnovu ulaznog podatka u obliku liste
struktura koje predstavljaju pojedini simulator ulaza.
Glavna dretva statički odabire zadatke, po offline algoritmu (zadanom prije pokretanja programa).

Glavna dretva pokreće timer dretvu, koja svakih 100 ms generira signal, kojim daje do znanja glavnoj dretvi da je vrijeme za novu periodu.

Glavna dretva tijekom obrade provjerava je li došlo do nove periode, ako je provjerava se treba li se prekinuti obrada prethodnog zadatka ili dopustiti prelazak u sljedeću 
periodu.

Kada dođe vrijeme za nove podatke, simulator ulaza simulira dolazak podataka te kada dođe na red glavnoj dretvi,
ona krene obrađivati te podatke. Glavna dretva pri završetku obrade javlja da je obrada gotova te simulator ulaza
prikuplja podatke o vremenu i uspješnosti obrade. Simulatori ulaza ovdje služe isključivo kako bi pratili vrijeme reackije te ostale podtake o zadacima.

Program se prekida uz pomoć signala SIGINT (CTRL + c), koji pokreće ispis podataka o obradama te gasi proces (sve dretve).
