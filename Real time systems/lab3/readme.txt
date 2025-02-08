Glavna dretva stvara N dretvi simulatora, gdje je N dobiven na osnovu ulaznog podatka u obliku liste
struktura koje predstavljaju pojedini simulator ulaza.

Svaki od simulatora pokreće svoju worker dretvu, koja čeka na dolazak posla kojeg simulira simulator ulaza. Worker simulira rad pomoću radnog čekanja.

Worker dretva pri završetku obrade javlja da je obrada gotova te simulator ulaza
prikuplja podatke o vremenu i uspješnosti obrade. Simulatori ulaza ovdje služe kako bi pratili vrijeme reackije te ostale podatke o zadacima.

Program se prekida uz pomoć signala SIGINT (CTRL + c), koji pokreće ispis podataka o obradama te gasi proces (sve dretve).
