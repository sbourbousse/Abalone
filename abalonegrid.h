#ifndef ABALONEGRID_H
#define ABALONEGRID_H

#include <QObject>
#include <QGridLayout>
#include "emplacement.h"
class Emplacement;
class AbaloneGrid : public QGridLayout
{
    Q_OBJECT
private:
    QString joueurActif;//rouge ou bleu
    QVector<Emplacement*>emplacementsSelectionnes;
public:
    AbaloneGrid();
    void changeJoueurActif();
    bool addEmplacementSelectionne(Emplacement * lEmplacement);
    bool pionsAlignes(Emplacement *lEmplacement);
    bool diagonale(Emplacement *lEmplacement);
    void retirerDeLaSelection(Emplacement*);
    bool existeEmplacementSelectionne(int abcisse,int ordonnee, Emplacement *);
    bool rendLaSelectionIncoherente(Emplacement* lEmplacement);
    int  poidsSelection(int dx,int dy);
    int resistanceSelection(int dx,int dy);
    void rangerLaSelection(int dx,int dy);
    bool existeEmplacementSelectionne(int abcisse, int ordonnee);
    bool dansLaBonneDirection(int dx, int dy);
    int resistance(Emplacement *lEmplacement, int dx, int dy);
    QString getJoueurActif() const;
    bool selectionEmpty();
    void permuter(Emplacement* e1,Emplacement * e2);
    Emplacement* getEmplacement(int abcisse, int ordonnee);
    void desactiverToutLesEmplacements();


signals:
    void joueurActifChange(QString);
    void selectionAChange();
    void bouleEjectee(QString);
    void clic(int,int);
public slots:
    void deplacerSelection();
};

#endif // ABALONEGRID_H
