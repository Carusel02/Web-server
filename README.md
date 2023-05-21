Tema4 PCOM Client Web

Implementare

-> scheletul de baza de la care am pornit in aceasta tema
a fost reprezentat de laboratorul 9 (HTML) de unde am
preluat fisierele de compunere a request urilor
-> de asemenea, m am folosit de biblioteca recomandata
in pdf pentru gestionarea obiectelor json
-> mi am definit mai multe functii auxiliare, care au 
urmatorul scop:
1) extract_cookie : extrage cookie ul primit de la un server
2) extract_token : extrage token ul primit de la server
3) create_user : creeaza un json cu informatiile necesare
pentru user
4) create_book : creeaza un json cu informatiile necesare
pentru crearea unei carti
5) notification_client : notifica clientul in functie de
input
-> in esenta, in functia main se citeste de fiecare data
ce comanda da utilizatorul si apoi se executa comanda coresp
Astfel, avem urmatoarele comenzi :
1) REGISTER = creeaza un json pentru utilizatorul care urm
sa fie inregistrat, facand o cerere de tip post la adresa
specificata in enunt
2) LOGIN = similar cu register, se schimba adresa
3) ENTER_LIBRARY = creeaza o cerere de tip get cu cookie ul
primit de la login, apoi se parseaza tokenul primit
4) GET_BOOKS = se creeaza tot o cere de tip get cu token ul
primit, apoi se parseaza lista de obiecte (carti) json
5) GET_BOOK = similar cu get_books, se concateneaza la url 
un id citit de user (avem nevoie de token)
6) ADD_BOOK = se creeaza un obiect json de tip carte, apoi
se face o cerere post (avem nevoie de token)
7) DELETE_BOOK = se citeste id ul cartii de la user si apoi se
face o cerere DELETE (avem nevoie de token)
8) LOGOUT = se face o cerere de tip get la adresa de logout,
se invalideaza cookie ul de logare si se sterge token ul din
memorie
9) EXIT = se iese din program
Pentru a ma asigura de o functionare buna a programului, am 
creat un enum cu state ul userului, pentru a controla flow ul
programului. Astfel, daca userul nu este logat, nu poate face
decat register, login si exit. Daca este logat, poate face 
orice alta comanda, iar daca este in biblioteca, poate face
orice alta comanda in afara de register si login.