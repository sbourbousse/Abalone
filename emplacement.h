#ifndef EMPLACEMENT_H
#define EMPLACEMENT_H

#include <QPushButton>
#include "abalonegrid.h"
class AbaloneGrid;
class Emplacement : public QPushButton
{
    Q_OBJECT
private:
    QString couleur;
    int abcisse;
    int ordonnee;
    bool isSelected;
    bool horsDuPlateau;
    AbaloneGrid * lePlateau;
public:
    Emplacement(AbaloneGrid* thePlateau,QString laCouleur,int abcisse,int ordonnee,QWidget* parent=nullptr);
    ~Emplacement();
    QString getCouleur();
    int getOrdonnee() const;
    int getAbcisse() const;
    void deselectionne();

    void setAbcisse(int value);

    void setOrdonnee(int value);

    void setCouleur(const QString &value);

    void justSetCouleur(const QString &value);
    void majImage();

public slots:
    void changeEtat();
};

#endif // EMPLACEMENT_H
