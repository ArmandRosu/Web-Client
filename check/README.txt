Roșu Armand-Alexandru, 321 CD
______________________________

Am început tema plecând de la laboratorul 9.

În request.c, se află cele două funcții de la lab, compute_get_request și
compute_post_request, cu modificări minore, alături de compute_delete_request.
Aceasta din urmă este aproape identică cu compute_get_request, singura diferenta fiind
liniile de sus, unde scriu DELETE in loc de GET.


Restul codului se află in client.c, unde citim comanda. Dacă este exit, se merge la
finalul codului, unde se elibereaza memoria si se inchide programul. Altfel, se incepe
deschizandu-se conexiunea socket-ului. Daca comanda este register, se citesc username-ul
si parola, se verifica sa nu aiba spatii, se construieste obiectul json, iar apoi se
verifica mesajul primit de la server (care ar trebui sa fie 200 sau 201).

Similar la login cu exit_code-ul 200, numai cu alta adresa la post_request (aceasta fiind
diferita de la comanda la comanda), alaturi de setarea si afisarea cookie-ului. La
enter_library, se trece direct la compute_get_request, in loc de compute_post_request,
si se pune si campul cookie, nu doar NULL, iar la final se afiseaza token-ul, in loc de
cookie.

La fel cu get_books_function, punandu-se si token-ul si afisandu-se cartile. La get_book
trebuie introdus id-ul cartii si se afiseaza doar cartea respectiva. In cazul lui
add_book, trebuie introduse campurile title, author, genre, publisher si page_count,
iar apoi se afiseaza 200, adica exit_code-ul bun, sau eroare (acesta fiind singura functie
unde mai este folosit compute_post_request).

Urmeaza delete_book_function, care cere id-ul cartii care trebuie stearsa si, in rest,
procedeaza la fel, dar cu compute_delete_request (singura functie unde este utilizata).
Ultima este functia logout, care doar da compute_get_request, send_to_Server,
receive_from_server si verifica exit_code-ul.