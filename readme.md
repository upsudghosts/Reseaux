# 1° Client/Server UDP chat room implémentée en C.

## Règles d'utilisation :

1)  Ouvrez trois fenêtres de terminal différentes et accéder au dossier UDP contenu dans Serveur Réseaux

2)  Dans une des fenêtres lancez la commande: `./serveur_udp`

3)  Dans les deux autres lancez une instance de client_udp en utilisant : `./client_udp <ip serveur>`
> Rappel : Votre adresse Ip ainsi que le nom de la machine s'affiche dans le terminal ou vous avez lancé le serveur.

4)  Une fois les messages de connexion apparus, vous êtes libres d'écrire vos messages qui apparaîtront dans le terminal du serveur.

---

# 2° Client/Server TCP chat room implémentée en C à l'aide de sockets POSIX threads (multithread).

## Règles d'utilisation :

1)  Ouvrez trois fenêtres de terminal différentes et accéder au dossier TCP contenu dans Serveur Réseaux

2)  Dans une  des fenêtre lancez la commande : `./serveur_tcp`

3)  Dans les deux autres lancez une instance de client_udp en utilisant : `./client_tcp <nom_machine>`
> Rappel : Votre adresse Ip ainsi que le nom de la machine s'affichent dans le terminal ou vous avez lancé le serveur.

4)  Les clients peuvent maintenant communiquer entre eux. Les messages s'afficheront avec les pseudonymes dans les terminaux clients.


**Le travail fourni fût rendu possible grâce à la documentation complète du [Guide pour la programmation réseaux de Beej's](http://vidalc.chez.com/lf/socket.html)**
