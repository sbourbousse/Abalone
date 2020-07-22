#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "abalonegrid.h"
#include "boutondirection.h"
#include <QDebug>
#include <QLCDNumber>
#include <QMessageBox>
#include <dialogconnexion.h>

class BoutonDirection;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //ajout de la grille abalone
    lePlateau=new AbaloneGrid();
    ui->verticalLayout->addLayout(lePlateau);

    //connection des signaux aux slots
    connect(lePlateau,SIGNAL(joueurActifChange(QString)),this, SLOT(changementJoueur(QString)));
    connect(lePlateau, SIGNAL(selectionAChange()),this, SLOT(activeDesactiveBtDirection()));
    //je remplis mon vecteur de BoutonDirection
    ui->pushButtonE->setDx(2);
    ui->pushButtonE->setDy(0);
    vectBoutonRouges.push_back(ui->pushButtonE);
    vectBoutonBleus.push_back(ui->pushButtonE);
    ui->pushButtonO->setDx(-2);
    ui->pushButtonO->setDy(0);
    vectBoutonRouges.push_back(ui->pushButtonO);
    vectBoutonBleus.push_back(ui->pushButtonO);

    ui->pushButtonNE->setDx(1);
    ui->pushButtonNE->setDy(-1);
    vectBoutonRouges.push_back(ui->pushButtonNE);

    ui->pushButtonNO->setDx(-1);
    ui->pushButtonNO->setDy(-1);
    vectBoutonRouges.push_back(ui->pushButtonNO);

    ui->pushButtonSE->setDx(1);
    ui->pushButtonSE->setDy(1);
    vectBoutonBleus.push_back(ui->pushButtonSE);

    ui->pushButtonSO->setDx(-1);
    ui->pushButtonSO->setDy(1);
    vectBoutonBleus.push_back(ui->pushButtonSO);


    foreach( BoutonDirection* leBouton, vectBoutonBleus)
    {
        connect(leBouton, SIGNAL(clicked()), lePlateau, SLOT(deplacerSelection()));
    }
    foreach( BoutonDirection* leBouton, vectBoutonRouges)
    {
        if(!vectBoutonBleus.contains(leBouton))
        connect(leBouton, SIGNAL(clicked()), lePlateau, SLOT(deplacerSelection()));
    }
    connect(lePlateau,SIGNAL(bouleEjectee(QString)),this, SLOT(majScore(QString)));
    changementJoueur("rouge");
    
    //j'alloue de la mémoire pour mon service
    monService=new QTcpServer(this);

}
MainWindow::~MainWindow()
{
    delete ui;
}

AbaloneGrid *MainWindow::getLePlateau() const
{
    return lePlateau;
}

void MainWindow::afficherGagnant(QString leGagnant)
{
    QMessageBox::information(this,"Abalone","The winner is: "+leGagnant);
}
/**
 * @brief MainWindow::changementJoueur
 * @param nouveauJoueur
 * active et désactive les boutons directionnels
 */
void MainWindow::changementJoueur(QString nouveauJoueur)
{
    qDebug()<<"void MainWindow::changementJoueur(QString nouveauJoueur)";
   //désactivation de tous les boutons de direction
    foreach(BoutonDirection* leBouton, vectBoutonBleus)
    {
        leBouton->setEnabled(false);
    }
    foreach(BoutonDirection* leBouton, vectBoutonRouges)
    {
        leBouton->setEnabled(false);
    }

    //Si c'est une partie en ligne et que c'est au tour de l'adversaire
    if(partieEnLigne && nouveauJoueur == "rouge"){
        //Desactiver les emplacements du plateau pour ce tour
        lePlateau->desactiverToutLesEmplacements();
    }


}
/**
 * @brief MainWindow::activeDesactiveBtDirection
 * ce slot est exécuté lorsque la sélection a changé
 * on doit activer ou désactiver les boutons de direction en fonction de la sélection et du joueur actif
 */
void MainWindow::activeDesactiveBtDirection()
{
    qDebug()<<"void MainWindow::activeDesactiveBtDirection()";
    //si aucune sélection n'a été faite on désactive tous les boutons
    if(lePlateau->selectionEmpty())
    {
        //On commence par les rouges
        foreach (BoutonDirection* leBouton, vectBoutonRouges)
        {
            leBouton->setEnabled(false);
        }
        //on fini par les bleus
        foreach (BoutonDirection* leBouton, vectBoutonBleus)
        {
            leBouton->setEnabled(false);
        }
    }
    else //il y a une sélection
    {
        //parcours des boutons de direction et test si dans la direction des boutons le déplacement est possible
        QVector<BoutonDirection*>vectBoutonDirectionActivables, vectBoutonsADesactiver;
        if(lePlateau->getJoueurActif()=="rouge")
        {
            vectBoutonDirectionActivables=vectBoutonRouges;
            vectBoutonsADesactiver=vectBoutonBleus;
        }
        else
        {
            vectBoutonDirectionActivables=vectBoutonBleus;
            vectBoutonsADesactiver=vectBoutonRouges;
        }
        //désactivation des autres boutons
        foreach (BoutonDirection* leBouton, vectBoutonsADesactiver)
        {
            leBouton->setEnabled(false);
        }
        //activation éventuelle des boutons propres au joueur actif
        foreach (BoutonDirection* leBouton, vectBoutonDirectionActivables)
        {
            //j'obtiens de dx et dy du bouton
            int dx=leBouton->getDx();
            int dy=leBouton->getDy();
            //obtention poids de la sélection et résistance
            int lePoids=lePlateau->poidsSelection(dx,dy);
            int laResistance=lePlateau->resistanceSelection(dx,dy);
            //pour debugger
            qDebug()<<"dans la direction"<<leBouton->text()<<" le poids est de: "<<lePoids<<" et la résistance est de: "<<laResistance;
            //activation si les conditions sont réunies
            leBouton->setEnabled(lePoids>laResistance);
        }
    }
}
/**
 * @brief MainWindow::majScore
 * @param joueurActif
 * met à jour les scores des joueurs et eventuellement affiche le gagnant
 */
void MainWindow::majScore(QString joueurActif)
{
    qDebug()<<"void MainWindow::majScore(QString joueurActif)";
    QLCDNumber * leLCDNumber;
    QLCDNumber * lAutreLCDNumber;
    if(joueurActif=="rouge")
    {
        leLCDNumber=ui->lcdNumberRouge;
        lAutreLCDNumber=ui->lcdNumberBleu;
    }
    else
    {
        leLCDNumber=ui->lcdNumberBleu;
        lAutreLCDNumber=ui->lcdNumberRouge;
    }
    leLCDNumber->display(leLCDNumber->value()+1);
    if(leLCDNumber->value()==6 ||leLCDNumber->value()-lAutreLCDNumber->value()==2 )
    {
        afficherGagnant(joueurActif);
        //fermer l'application
        close();
    }

}
/**
 * @brief MainWindow::connexionATraiter
 * cette méthode est exécutée lorsque un joueur se connecte à un autre sur le port 11111
 */
void MainWindow::connexionATraiter()
{
    qDebug()<<"void MainWindow::connexionATraiter()";
    //je gère la connexion
    laSocketDeCom=monService->nextPendingConnection();
    laSocketDeCom->write("WELCOME\r\n\r\n");
    connect(laSocketDeCom, SIGNAL(readyRead()),this, SLOT(traiteMessageRecu()));
    partieEnLigne = true;
    lePlateau->desactiverToutLesEmplacements();
}
/**
 * @brief MainWindow::traiteMessageRecu
 * cette méthode est exécutée quand un message arrive dans la socket
 * Rappel protocole:
 * PSEUDO raoul
 * CLIC 12,3
 */
void MainWindow::traiteMessageRecu()
{
    qDebug()<<"void MainWindow::traiteMessageRecu()";
    QString messageRecu=laSocketDeCom->readAll();
    qDebug()<<messageRecu;
    //analyse du message reçu
    QStringList listeDesMotsDuMessage=messageRecu.split(" ");
    QString laCommande=listeDesMotsDuMessage[0];
    QString leReste=listeDesMotsDuMessage[1];
    //si la commande est PSEUDO
    if(laCommande=="PSEUDO")
    {
        //on va afficher le pseudo qlq part
        ui->labelAdversaire->setText(leReste);
    }
    else
    {
        //ce peut être un clic ou un bouton de direction
        if(laCommande=="CLIC")
        {
            //c'est un clic il faut que je répercute ça sur mon plateau
            //J'attends vos propositions
            //extraire les coordonnées du message
            QString lesCoordo=leReste.split("\r")[0];
            QStringList listeCoordo=lesCoordo.split(",");
            int abcisse=listeCoordo[0].toInt();
            int ordonnee=listeCoordo[1].toInt();
            //chopper la bille correspondante
            Emplacement * lEmplacement=lePlateau->getEmplacement(abcisse,ordonnee);
            //lui faire émettre le signal clicked
            lEmplacement->click();
        }
        else
        {
            if(laCommande=="BOUTON")
            {
                BoutonDirection* boutonConcerne=nullptr;

                if(leReste.left(2)=="NO")
                {
                    boutonConcerne=ui->pushButtonNO;
                }
                else if (leReste.left(2)=="NE")
                {
                    boutonConcerne=ui->pushButtonNE;
                }
                else if (leReste.left(2)=="E")
                {
                    boutonConcerne=ui->pushButtonE;
                }
                else if (leReste.left(2)=="O")
                {
                    boutonConcerne=ui->pushButtonO;
                }
                else if (leReste.left(2)=="SE")
                {
                    boutonConcerne=ui->pushButtonSE;
                }
                else if (leReste.left(2)=="SO")
                {
                    boutonConcerne=ui->pushButtonSO;
                }
                if(boutonConcerne!=nullptr)
                    boutonConcerne->click();
            }
        }
    }
}


/**
 * @brief MainWindow::on_toolButtonServeur_clicked
 * cette méthode est éxécutée quand un abalonien lance son serveur en cliquant sur le bouton
 */
void MainWindow::on_toolButtonServeur_clicked()
{
    qDebug()<<"void MainWindow::on_toolButtonServeur_triggered(QAction *arg1)";
    connect(monService,SIGNAL(newConnection()), this, SLOT(connexionATraiter()));
    monService->listen(QHostAddress::Any,11111);
    monService->waitForNewConnection();
}

void MainWindow::on_pushButtonConnexion_clicked()
{
    qDebug()<<"void MainWindow::on_pushButtonConnexion_clicked()";
    DialogConnexion theDialog;
    //Si la fenêtre a été fermé avec le bouton de connexion
    if(theDialog.exec() == QDialog::Accepted) {
        QString pseudo=theDialog.getPseudo();
        QString ip=theDialog.getIp();
        //création de la socket vers le serveur
        laSocketDeCom=new QTcpSocket(this);
        laSocketDeCom->connectToHost(ip,11111);
        if (laSocketDeCom->waitForConnected()){
            qDebug()<<"Connexion à l'hôte réussie";
            partieEnLigne = true;
            QString message = "PSEUDO "+pseudo;
            //Envoyer son pseudo à l'adversaire en respectant le protocole
            laSocketDeCom->write(message.toUtf8());
            //effectuer la même modification pour la mainwindow local
            ui->labelJoueurLocal->setText(pseudo);
            connect(lePlateau, SIGNAL(clic(int,int)),this, SLOT(envoyerModifSelection(int,int)));
            foreach(BoutonDirection* leBouton, vectBoutonBleus)
                connect(leBouton, SIGNAL(direction(QString)),this, SLOT(envoyerDeplacerSelection(QString)));

            foreach(BoutonDirection* leBouton, vectBoutonRouges)
                connect(leBouton, SIGNAL(direction(QString)),this, SLOT(envoyerDeplacerSelection(QString)));
        }
        else
            qDebug()<<"Connexion échouée";
    }
}

void MainWindow::envoyerModifSelection(int abscisse, int ordonne)
{
    qDebug()<<"void MainWindow::envoyerModifSelection()";
    //Trouver les coordonnées du boutons qui a été cliqué
    QString leMessage = "CLIC "+QString::number(abscisse).toUtf8()+","+QString::number(ordonne).toUtf8();
    laSocketDeCom->write(leMessage.toUtf8());
    qDebug()<<leMessage;
}

void MainWindow::envoyerDeplacerSelection(QString direction)
{
    qDebug()<<"void MainWindow::envoyerDeplacerSelection(QString direction)";
    QString leMessage = "BOUTON "+direction;
    laSocketDeCom->write(leMessage.toUtf8());
    qDebug()<<leMessage;
}
