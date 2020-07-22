#include "emplacement.h"
#include <QDebug>

int Emplacement::getOrdonnee() const
{
    return ordonnee;
}

int Emplacement::getAbcisse() const
{
    return abcisse;
}
/**
 * @brief Emplacement::deselectionne
 * passe le booléen isSelected à false
 * et change l'icône
 */
void Emplacement::deselectionne()
{
    isSelected=false;
    QString nomIcone=":images/"+couleur;
    setIcon(QIcon(QPixmap(nomIcone)));

}
/**
 * @brief Emplacement::Emplacement
 * @param thePlateau
 * @param laCouleur
 * @param lAbcisse
 * @param lOrdonnee
 * @param parent
 * constructeur de l'emplacement
 */
void Emplacement::setAbcisse(int value)
{
    abcisse = value;
}

void Emplacement::setOrdonnee(int value)
{
    ordonnee = value;
}

void Emplacement::setCouleur(const QString &value)
{
    couleur = value;
    setIcon(QIcon(QPixmap(":images/"+couleur)));
}
void Emplacement::justSetCouleur(const QString& value)
{
    couleur=value;
}

Emplacement::Emplacement(AbaloneGrid *thePlateau, QString laCouleur, int lAbcisse, int lOrdonnee, QWidget *parent):QPushButton(parent)
{
  qDebug()<<"Emplacement::Emplacement(QString laCouleur, int lAbcisse, int lOrdonnee, QWidget *parent):QPushButton(parent)";
  isSelected=false;
  horsDuPlateau=false;
  couleur=laCouleur;
  abcisse=lAbcisse;
  ordonnee=lOrdonnee;
  lePlateau=thePlateau;
  setIcon(QIcon(QPixmap(":images/"+couleur)));
  setEnabled(!(couleur=="vide"));
  //seules les billes sont sélectionnables
  if(couleur!="vide")
      connect(this,SIGNAL(clicked()),this,SLOT(changeEtat()));
}

Emplacement::~Emplacement()
{
   //rien à faire pour le moment
}
/**
 * @brief Emplacement::getCouleur
 * @return laCouleur de l'emplacement bleu rouge ou vide
 */
QString Emplacement::getCouleur()
{
 return couleur;
}
void Emplacement::majImage()
{
    QString nomIcone=":images/"+couleur;
    if(isSelected)
    {
        nomIcone+="Sel";
    }
    else
    {
        //virer le pépère de la selection
        lePlateau->retirerDeLaSelection(this);
    }
    setIcon(QIcon(QPixmap(nomIcone)));
}

/**
 * @brief Emplacement::changeEtat
 * sélectionne ou déselectionne un emplacement
 * cela change son icône le rajoute ou le retire éventuellement à la sélection
 */
void Emplacement::changeEtat()
{
    qDebug()<<"void Emplacement::changeEtat()";
    if(isSelected || lePlateau->addEmplacementSelectionne(this))
    {
        isSelected=!isSelected;
        QString nomIcone=":images/"+couleur;
        if(isSelected)
        {
            nomIcone+="Sel";
        }
        else
        {
            //virer le pépère de la selection
            lePlateau->retirerDeLaSelection(this);
        }
        setIcon(QIcon(QPixmap(nomIcone)));
    }
    if(lePlateau->getJoueurActif() == couleur) {
        lePlateau->emit clic(abcisse,ordonnee);
    }
}


