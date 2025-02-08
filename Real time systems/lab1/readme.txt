Glavna dretva stvara N dretvi simulatora, gdje je N dobiven na osnovu ulaznog podatka u obliku liste
struktura koje predstavljaju pojedini simulator ulaza.

Glavna dretva kontinuirano ažurira vrijeme te provjerava je li došlo novih podataka za obradu na nekog od ulaza.
Kada dođe vrijeme za nove podatke, simulator ulaza simulira dolazak podataka te kada dođe na red glavnoj dretvi,
ona krene obrađivati te podatke. Glavna dretva pri završetku obrade javlja da je obrada gotova te simulator ulaza
prikuplja podatke o vremenu i uspješnosti obrade.

Program se prekida uz pomoć signala SIGINT (CTRL + c), koji pokreće ispis podataka o obradama te gasi proces (sve dretve).
