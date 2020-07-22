#include "abalonegrid.h"
#include "emplacement.h"
#include "boutondirection.h"
#include <QDebug>
/**
 * @brief AbaloneGrid::AbaloneGrid
 * crée tous les pions rouges ou bleus et les place au bon endroit
 * donne le premier tour au joueur rouge.
 */
QString AbaloneGrid::getJoueurActif() const
{
    return joueurActif;
}

bool AbaloneGrid::selectionEmpty()
{
    return emplacementsSelectionnes.isEmpty();
}
/**
 * @brief AbaloneGrid::permuter
 * @param e1 un emplacement
 * @param e2  un autre emplacement
 * permute les deux emplacements passés en paramètre
 * statut -> non testée
 */
void AbaloneGrid::permuter(Emplacement *e1, Emplacement *e2)
{
    qDebug()<<"void AbaloneGrid::permuter(Emplacement *e1, Emplacement *e2)";
    //Emplacement* temp=new Emplacement(this,e1->getCouleur(),e1->getAbcisse(),e1->getOrdonnee());
    //*e2=*e1;

    //effacement des 2 anciens emplacement
    removeWidget(e2);
    removeWidget(e1);
    //permute dans la grille
    addWidget(e2,e1->getOrdonnee(),e1->getAbcisse());
    addWidget(e1,e2->getOrdonnee(),e2->getAbcisse());
    int xe1,ye1;
    QString couleurE1;
    //on sauve abcisse et ordonnee de e1 //ainsi que son image
    xe1=e1->getAbcisse();
    ye1=e1->getOrdonnee();
    //couleurE1=e1->getCouleur();

    //travail sur e1
    e1->setAbcisse(e2->getAbcisse());
    e1->setOrdonnee(e2->getOrdonnee());
    //e1->justSetCouleur(e2->getCouleur());
    //e1->majImage();
    //travail sur e2
    e2->setAbcisse(xe1);
    e2->setOrdonnee(ye1);
    //e2->justSetCouleur(couleurE1);
    //e2->majImage();
}
/**
 * @brief AbaloneGrid::deplacerSelection
 * déplace les billes sélectionnées
 */
void AbaloneGrid::deplacerSelection()
{
    qDebug()<<"void AbaloneGrid::deplacerSelection()";
    BoutonDirection* leBoutonClique=qobject_cast<BoutonDirection*>(sender());
    int dx=leBoutonClique->getDx();
    int dy=leBoutonClique->getDy();
    qDebug()<<leBoutonClique->text()<<" dx:"<<dx<<" dy:"<<dy;
    //déplacer les boules sélectionnées
    //s'il s'agit d'un déplacement en ligne
    if(resistanceSelection(dx,dy)==0 && poidsSelection(dx,dy)==1)
    {
        qDebug()<<"déplacement en ligne";
        //alors déplacer tous les boutons de la sélection
        foreach(Emplacement* lEmplacement , emplacementsSelectionnes)
        {
            //obtenir le next
            Emplacement* suivant=qobject_cast<Emplacement*>(itemAtPosition(lEmplacement->getOrdonnee()+dy,lEmplacement->getAbcisse()+dx)->widget());
            //permutation
            permuter(lEmplacement,suivant);
        }
    }

    //sinon
    else
    {
        qDebug()<<"déplacement classique";
        //obtention du suivant
        Emplacement* lastBoule=emplacementsSelectionnes.at(emplacementsSelectionnes.count()-1);
        Emplacement* suivant=qobject_cast<Emplacement*>(itemAtPosition(lastBoule->getOrdonnee()+leBoutonClique->getDy(),lastBoule->getAbcisse()+leBoutonClique->getDx())->widget());
        if(suivant->getCouleur()=="vide")
        {
            //permuter la première boule avec l'emplacement vide en mettant à jour les coordonnées
            permuter(suivant,emplacementsSelectionnes[0]);
        }
        else
        {
            //il y a des boules à pousser (2 permutations sont nécessaires
            //<------c'est là que l'on bosse
            //première permutation la suivante avec l'emplacement libre derrière l'adversaire
            qDebug()<<"Attention poussée en cours";
            QLayoutItem* lItem=itemAtPosition(suivant->getOrdonnee()+dy*resistanceSelection(dx,dy),suivant->getAbcisse()+dx*resistanceSelection(dx,dy));

            //si il y a rien derrière bord du plateau atteint
            if(lItem==nullptr)
            {
                //on éjecte la boule suivante
                suivant->setCouleur("vide");
                //on rajoute 1 au score du joueur actif
                emit(bouleEjectee(joueurActif));
            }
            else//du vide derrière
            {
                Emplacement * derriere=qobject_cast<Emplacement*>(lItem->widget());
                permuter(suivant,derriere);
            }
            //deuxième permutation
            //je reprends l'objet suivant
            suivant=qobject_cast<Emplacement*>(itemAtPosition(lastBoule->getOrdonnee()+leBoutonClique->getDy(),lastBoule->getAbcisse()+leBoutonClique->getDx())->widget());
            permuter(suivant,emplacementsSelectionnes[0]);
        }
    }
    changeJoueurActif();
    foreach(Emplacement* lEmplacement, emplacementsSelectionnes)
    {
        lEmplacement->deselectionne();
    }
    emplacementsSelectionnes.clear();
}

AbaloneGrid::AbaloneGrid():QGridLayout()
{
    qDebug()<<"AbaloneGrid::AbaloneGrid(QWidget* parent):QGridLayout(parent)";
    //première ligne
    int colonneDepart=4;
    int nbBoutons=5;
    //nous avons 9 lignes
    for(int ligne=0;ligne<9;ligne++)
    {
        //boucle de création des boutons
        for(int col=colonneDepart;col<colonneDepart+nbBoutons*2;col+=2)
        {
            //valorisation des propriétés de l'emplacement
            //couleur du bouton
            QString couleur="vide";
            if(ligne<2 || (ligne==2 && (col<11 && col>5)))
            {
                couleur="bleu";
            }
            else
            {
                if(ligne>6 || (ligne==6 && (col<11 && col>5)))
                {
                    couleur="rouge";
                }
            }
            //on cree l'emplacement
            Emplacement * nouvelEmplacement=new Emplacement(this,couleur,col,ligne);
            addWidget(nouvelEmplacement,ligne,col);
        }//fin de la création des boutons
        //maj de la position de départ et du nombre de bouton à dessiner
        if(ligne<4)//partie croissante
        {
            nbBoutons++;
            colonneDepart--;
        }
        else//partie décroissante
        {
            nbBoutons--;
            colonneDepart++;
        }
    }//fin du pour chaque ligne
    //sélection du joueur actif
    joueurActif="bleu";
    //c'est le rouge qui commence
    changeJoueurActif();
}
/**
 * @brief AbaloneGrid::changeJoueurActif
 * change le joueur actif désactive les pions du joueur précédent
 * et active les pions du joueur dont c'est le tour
 */
void AbaloneGrid::changeJoueurActif()
{
    qDebug()<<"void AbaloneGrid::changeJoueurActif()";
    //disable des boutons du joueur actif et enable des boutons de l'autre
    //il faut tout parcourir
    for(int ligne=0;ligne<rowCount();ligne++)
    {
        for(int col=0;col<columnCount();col++)
        {
            qDebug()<<"ligne:"<<ligne<<"colonne"<<col;
            QLayoutItem* item=itemAtPosition(ligne,col);
            //itemAtPosition renvoie un pointeur null si il n'y a rien à cet emplacement
            if(item!=nullptr)
            {
                Emplacement* lEmplacement=static_cast<Emplacement*>(item->widget());
                if(lEmplacement->getCouleur()==joueurActif)
                {
                    lEmplacement->setEnabled(false);
                }
                else
                {
                    if(lEmplacement->getCouleur()!="vide")
                    {
                        lEmplacement->setEnabled(true);
                    }
                }
            }
        }
    }
    //chgt du joueur actif ternaire variable=(condition)?valeurSiVrai:valeurSiFaux
    joueurActif =(joueurActif=="rouge")?"bleu":"rouge";
    //émission d'un événement
    emit joueurActifChange(joueurActif);
}

/**
 * @brief AbaloneGrid::pionsAlignes
 * @param lEmplacement
 * @return vrai si tous les emplacements sont en ligne horizontale faux sinon
 */
bool AbaloneGrid::pionsAlignes(Emplacement *lEmplacement)
{
    qDebug()<<"bool AbaloneGrid::pionsAlignes(Emplacement *lEmplacement)";
    //on regarde si l'écart d'abcisse est de deux
    bool jouxte=abs(lEmplacement->getAbcisse()-emplacementsSelectionnes.at(0)->getAbcisse())==2;
    //ont-ils la même ordonnée?
    bool alignes= lEmplacement->getOrdonnee()==emplacementsSelectionnes.at(0)->getOrdonnee();
    if(emplacementsSelectionnes.count()==2)
    {
        alignes=alignes && lEmplacement->getOrdonnee()==emplacementsSelectionnes.at(1)->getOrdonnee();
        jouxte=jouxte || abs(lEmplacement->getAbcisse()-emplacementsSelectionnes.at(1)->getAbcisse())==2;
    }
    return jouxte && alignes;
}
/**
 * @brief AbaloneGrid::diagonale
 * @param lEmplacement
 * @return vrai si les pions sélectionnés sont en diagonale faux sinon
 */
bool AbaloneGrid::diagonale(Emplacement* lEmplacement)
{
    qDebug()<<"bool AbaloneGrid::diagonale(Emplacement* lEmplacement)";
    bool resultat, aCoteDuPremier=false;
    //2 emplacements
    if(emplacementsSelectionnes.count()==1)
    {
        aCoteDuPremier= abs(lEmplacement->getAbcisse()-emplacementsSelectionnes.at(0)->getAbcisse())==1;
        aCoteDuPremier=aCoteDuPremier &&abs(lEmplacement->getOrdonnee()-emplacementsSelectionnes.at(0)->getOrdonnee())==1;
    }
    bool auMilieu=false;
    //3 emplacements
    if(emplacementsSelectionnes.count()==2)
    {
        //1 des emplacements doit être au milieu des deux autres
        //calcul du milieu
        double abcisseMilieu=(emplacementsSelectionnes.at(1)->getAbcisse()+emplacementsSelectionnes.at(0)->getAbcisse()+lEmplacement->getAbcisse())/3.0;
        double ordonneeMilieu=(emplacementsSelectionnes.at(1)->getOrdonnee()+emplacementsSelectionnes.at(0)->getOrdonnee()+lEmplacement->getOrdonnee())/3.0;
        auMilieu=abcisseMilieu-(int)abcisseMilieu==0 && ordonneeMilieu-(int)ordonneeMilieu==0 && existeEmplacementSelectionne((int)abcisseMilieu,(int)ordonneeMilieu,lEmplacement);
    }
    resultat=aCoteDuPremier || auMilieu;
    return resultat;
}
/**
 * @brief AbaloneGrid::rendLaSelectionIncoherente
 * @param lEmplacement
 * @return vrai si désélectionner l'emplacement passé en paramètre rend incohérente la sélection faux sinon
 * appelée lors de la déselection d'une des billes sélectionnées
 */
bool AbaloneGrid::rendLaSelectionIncoherente(Emplacement *lEmplacement)
{
    qDebug()<<"bool AbaloneGrid::rendLaSelectionIncoherente(Emplacement *lEmplacement)";
    if(emplacementsSelectionnes.count()<3)
        return false;
    else//trois sont sélectionnés
    {
        int sommeAbcisse=0;
        int sommeOrdonnee=0;
        for(int noEmp=0;noEmp<3;noEmp++)
        {
            sommeAbcisse+=emplacementsSelectionnes.at(noEmp)->getAbcisse();
            sommeOrdonnee+=emplacementsSelectionnes.at(noEmp)->getOrdonnee();
        }
        return lEmplacement->getAbcisse()==sommeAbcisse/3 && lEmplacement->getOrdonnee()==sommeOrdonnee/3;
    }
}
/**
 * @brief AbaloneGrid::poidsSelection
 * @param dx
 * @param dy
 * @return le poids de la sélection
 * testée OK
 */
int AbaloneGrid::poidsSelection(int dx, int dy)
{
    qDebug()<<"int AbaloneGrid::poidsSelection(int dx, int dy)";
    if(emplacementsSelectionnes.length()==1)
    {
        return 1;
    }
    else
    {
        if (dansLaBonneDirection(dx,dy))
            return emplacementsSelectionnes.length();
        else
            return 1;
    }
}
/**
 * @brief AbaloneGrid::resistance
 * @param dx
 * @param dy
 * @return la résistance de 3 à 0
 */
int AbaloneGrid::resistance(Emplacement* lEmplacement,int dx, int dy)
{
    qDebug()<<"int AbaloneGrid::resistance(Emplacement* lEmplacement,int dx, int dy)";
    //trouver les coordonnées du suivant
    int xSuiv=lEmplacement->getAbcisse()+dx;
    int ySuiv=lEmplacement->getOrdonnee()+dy;
    //essais d'obtention du suivant
    QLayoutItem* item=itemAtPosition(ySuiv,xSuiv);
    //itemAtPosition renvoie un pointeur null si il n'y a rien à cet emplacement
    if(item!=nullptr)
    {
        qDebug()<<"pas bord";
        Emplacement* suivant=static_cast<Emplacement*>(item->widget());
        if(suivant->getCouleur()==joueurActif)
        {
            return 3;
        }
        else
        {
            if(suivant->getCouleur()=="vide")
            {
                return 0;
            }
            else
            {
                return 1+resistance(suivant,dx,dy);
            }
        }
    }
    else //n'existe pas
    {
        //il ne doit pas sauter dans le vide
        if(lEmplacement->getCouleur()==joueurActif) return 3;
        //mais il peut y pousser son adversaire
        else return 0;
    }
}

/**
 * @brief AbaloneGrid::resistanceSelection
 * @param dx
 * @param dy
 * @return la résistance de 3 à 0
 */
int AbaloneGrid::resistanceSelection(int dx, int dy)
{
    qDebug()<<"int AbaloneGrid::resistanceSelection(int dx, int dy)";
    //rangement des billes en fonction de la direction
    rangerLaSelection(dx,dy);
    //dans le cas ou le déplacement ne se fait pas dans le sens de la sélection
    //la résistance est égale au max de la résistance opposée à chaque bille de la sélection
    if(poidsSelection(dx,dy)==1 && emplacementsSelectionnes.count()>1)
    {
        int maxResistance=resistance(emplacementsSelectionnes[0],dx,dy);
        foreach (Emplacement* lEmplacement, emplacementsSelectionnes)
        {
            //obtention de la résistance oposée à la boule en cours d'examen
            int resistBoule=resistance(lEmplacement,dx,dy);
            if(resistBoule>maxResistance)
            {
                maxResistance=resistBoule;
            }
        }//fin du foreach emplacement
        return maxResistance;
    }

    //trouver les coordonnées du suivant
    int xSuiv=emplacementsSelectionnes[emplacementsSelectionnes.count()-1]->getAbcisse()+dx;
    int ySuiv=emplacementsSelectionnes[emplacementsSelectionnes.count()-1]->getOrdonnee()+dy;
    //essais d'obtention du suivant
    QLayoutItem* item=itemAtPosition(ySuiv,xSuiv);
    //itemAtPosition renvoie un pointeur null si il n'y a rien à cet emplacement
    if(item!=nullptr)
    {
        Emplacement* lEmplacement=static_cast<Emplacement*>(item->widget());
        if(lEmplacement->getCouleur()==joueurActif)
        {
            return 3;
        }
        else
        {
            if(lEmplacement->getCouleur()=="vide")
            {
                return 0;
            }
            else
            {
                return 1+resistance(lEmplacement,dx,dy);//récursivité
            }
        }
    }
    else //n'existe pas
    {
        return 3;
    }
}
/**
 * @brief AbaloneGrid::rangerLaSelection
 * @param dx
 * @param dy
 * range la sélection de la dernière boule à la première boule dans le vecteur
 * la dernière boule du vecteur est celle qui est la première à bouger lors d'un deplacement
 * elle permet de trouver le suivant où elle doit aller
 */
void AbaloneGrid::rangerLaSelection(int dx, int dy)
{
    qDebug()<<"void AbaloneGrid::rangerLaSelection(int dx"<<dx<<", int dy"<<dy<<")";
    if(emplacementsSelectionnes.count()>1)
    {
        if(emplacementsSelectionnes.count()==2)
        {
            if(!(emplacementsSelectionnes[0]->getAbcisse()+dx==emplacementsSelectionnes[1]->getAbcisse()&&emplacementsSelectionnes[0]->getOrdonnee()+dy==emplacementsSelectionnes[1]->getOrdonnee()))
            {
                //permutation
                Emplacement * temp;
                temp=emplacementsSelectionnes[0];
                emplacementsSelectionnes[0]=emplacementsSelectionnes[1];
                emplacementsSelectionnes[1]=temp;
            }
        }
        else
        {
            //trois emplacements
            //Nord
            if (dy==-1)
            {

                bool permutation=true;
                while (permutation)
                {
                    permutation=false;
                    for (int i=0;i<2;i++)
                    {
                        //bug ici
                        if (emplacementsSelectionnes[i]->getOrdonnee()<emplacementsSelectionnes[i+1]->getOrdonnee())
                        {
                            Emplacement* temp=emplacementsSelectionnes[i+1];
                            emplacementsSelectionnes[i+1]=emplacementsSelectionnes[i];
                            emplacementsSelectionnes[i]=temp;
                            permutation=true;
                        }
                    }
                }
            }
            else if (dy==1) //sud
            {
                bool permutation=true;
                while (permutation)
                {
                    permutation=false;
                    for (int i=0;i<2;i++)
                    {
                        if (emplacementsSelectionnes[i]->getOrdonnee()>emplacementsSelectionnes[i+1]->getOrdonnee())//modif
                        {
                            Emplacement* temp=emplacementsSelectionnes[i+1];
                            emplacementsSelectionnes[i+1]=emplacementsSelectionnes[i];
                            emplacementsSelectionnes[i]=temp;
                            permutation=true;
                        }
                    }
                }
            }
            else if (dx==2 && dy==0)//=="est":
            {
                bool permutation=true;
                while (permutation)
                {
                    permutation=false;
                    for (int i=0;i<2;i++)
                    {
                        if (emplacementsSelectionnes[i]->getAbcisse()>emplacementsSelectionnes[i+1]->getAbcisse())
                        {
                            Emplacement* temp=emplacementsSelectionnes[i+1];
                            emplacementsSelectionnes[i+1]=emplacementsSelectionnes[i];
                            emplacementsSelectionnes[i]=temp;
                            permutation=true;
                        }
                    }
                }
            }
            else if (dx==-2 && dy==0)//direction=="ouest":
            {
                bool permutation=true;
                while (permutation)
                {
                    permutation=false;
                    for (int i=0;i<2;i++)
                    {
                        if (emplacementsSelectionnes[i]->getAbcisse()<emplacementsSelectionnes[i+1]->getAbcisse())
                        {
                            Emplacement* temp=emplacementsSelectionnes[i+1];
                            emplacementsSelectionnes[i+1]=emplacementsSelectionnes[i];
                            emplacementsSelectionnes[i]=temp;
                            permutation=true;
                        }
                    }
                }
            }
        }
    }
}
/**
 * @brief AbaloneGrid::dansLaBonneDirection
 * @param dx
 * @param dy
 * @return vrai si la sélection est dans la bonne direction
 * travaille uniquement sur une sélection rangée ne fonctionne pas sinon
 */
bool AbaloneGrid::dansLaBonneDirection(int dx, int dy)
{
    qDebug()<<"bool AbaloneGrid::dansLaBonneDirection(int dx, int dy)";
    rangerLaSelection(dx,dy);
    if(emplacementsSelectionnes.length()==2)
    {
        int xsuiv=emplacementsSelectionnes[0]->getAbcisse()+dx;
        int ysuiv=emplacementsSelectionnes[0]->getOrdonnee()+dy;
        return emplacementsSelectionnes[1]->getAbcisse()==xsuiv && emplacementsSelectionnes[1]->getOrdonnee()==ysuiv;
    }
    else
    {
        int sommeAbs=0;
        int sommeOrd=0;
        //3 boules sélectionnees
        for(int noEmp=0;noEmp<3;noEmp++)
        {
            sommeAbs+=emplacementsSelectionnes[noEmp]->getAbcisse();
            sommeOrd+=emplacementsSelectionnes[noEmp]->getOrdonnee();
        }
        int milieuX=sommeAbs/3;
        int milieuY=sommeOrd/3;
        return existeEmplacementSelectionne(milieuX+dx,milieuY+dy);
    }
}

/**
 * @brief AbaloneGrid::retirerDeLaSelection
 * @param lEmplacement c'est l'emplacement à retirer
 * cette procédure retire l'emplacement de la sélection
 * si la sélection devient incohérente alors toute la sélection est annulée
 */
void AbaloneGrid::retirerDeLaSelection(Emplacement * lEmplacement)
{  
    qDebug()<<"void AbaloneGrid::retirerDeLaSelection(Emplacement * lEmplacement)";
    //regarder si la sélection reste cohérente
    if(rendLaSelectionIncoherente(lEmplacement))
    {
        //tout déselectionner
        for(int noEmp=0;noEmp<3;noEmp++)
        {
            emplacementsSelectionnes.at(noEmp)->deselectionne();
        }
        //vider la sélection entière
        emplacementsSelectionnes.clear();
    }
    else
        emplacementsSelectionnes.removeOne(lEmplacement);
    //on signale ici que la sélection a changé
    emit(selectionAChange());
}
/**
 * @brief AbaloneGrid::existeEmplacementSelectionne
 * @param abcisse
 * @param ordonnee
 * @param nouvelEmplacement
 * @return vrai si l'emplacement dont les coordonnées sont passées en paramètre est sélectionné
 */
bool AbaloneGrid::existeEmplacementSelectionne(int abcisse, int ordonnee, Emplacement* nouvelEmplacement)
{
    qDebug()<<"bool AbaloneGrid::existeEmplacementSelectionne(int abcisse, int ordonnee, Emplacement* nouvelEmplacement)";
    bool existe=nouvelEmplacement->getAbcisse()==abcisse&&nouvelEmplacement->getOrdonnee()==ordonnee;
    int noCase=0;
    while(!(existe || noCase==2))
    {
        existe=emplacementsSelectionnes.at(noCase)->getAbcisse()==abcisse && emplacementsSelectionnes.at(noCase)->getOrdonnee()==ordonnee;
        noCase++;
    }
    return existe;
}
/**
 * @brief AbaloneGrid::existeEmplacementSelectionne
 * @param abcisse
 * @param ordonnee
 * @param nouvelEmplacement
 * @return vrai si l'emplacement dont les coordonnées sont passées en paramètre est sélectionné
 */
bool AbaloneGrid::existeEmplacementSelectionne(int abcisse, int ordonnee)
{
    qDebug()<<"bool AbaloneGrid::existeEmplacementSelectionne(int abcisse, int ordonnee)";
    bool existe=false;
    int noCase=0;
    while(!(existe || noCase>2))
    {
        existe=emplacementsSelectionnes.at(noCase)->getAbcisse()==abcisse && emplacementsSelectionnes.at(noCase)->getOrdonnee()==ordonnee;
        noCase++;
    }
    return existe;
}
/**
 * @brief AbaloneGrid::addEmplacementSelectionne
 * @param lEmplacement emplacement à ajouter
 * @return vrai si l'emplacement a pû être ajouté faux sinon
 */
bool AbaloneGrid::addEmplacementSelectionne(Emplacement *lEmplacement)
{
    qDebug()<<"bool AbaloneGrid::addEmplacementSelectionne(Emplacement *lEmplacement)";
    bool ajoutPossible=false;
    //si il n'y a pas encore de bille sélectionnée
    if(emplacementsSelectionnes.empty())
    {
        ajoutPossible=true;
    }
    else
    {
        //si c'est plein
        if(emplacementsSelectionnes.count()==3)
        {
            ajoutPossible=false;
        }
        else//1 ou 2 billes dans la sélection
        {
          ajoutPossible=pionsAlignes(lEmplacement)|| diagonale(lEmplacement);
        }
    }
    if(ajoutPossible)
    {
        emplacementsSelectionnes.push_back(lEmplacement);
        //émission d'un signal pour dire que la sélection a changé
        emit (selectionAChange());
    }
    return ajoutPossible;
}

Emplacement *AbaloneGrid::getEmplacement(int abcisse, int ordonnee)
{
    return static_cast<Emplacement*> (itemAtPosition(ordonnee,abcisse)->widget());
}

void AbaloneGrid::desactiverToutLesEmplacements()
{
    //première ligne
    int colonneDepart=4;
    int nbBoutons=5;

    //nous avons 9 lignes
    for(int ligne=0;ligne<9;ligne++)
    {
        //boucle de création des boutons
        for(int col=colonneDepart;col<colonneDepart+nbBoutons*2;col+=2)
        {
            //On desactive l'emplacement
            getEmplacement(col,ligne)->deselectionne();
        }//fin de la création des boutons

        //maj de la position de départ et du nombre de bouton à dessiner
        if(ligne<4)//partie croissante
        {
            nbBoutons++;
            colonneDepart--;
        }
        else//partie décroissante
        {
            nbBoutons--;
            colonneDepart++;
        }
    }
}
