#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "abalonegrid.h"
#include "boutondirection.h"
#include <QVector>
#include <QTcpServer>
#include <QTcpSocket>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    AbaloneGrid *getLePlateau() const;

private:
    Ui::MainWindow *ui;
    AbaloneGrid * lePlateau;
    QVector <BoutonDirection*> vectBoutonRouges,vectBoutonBleus;
    void afficherGagnant(QString leGagnant);
    QTcpServer* monService;
    QTcpSocket* laSocketDeCom;
    bool partieEnLigne;
private slots:
    void changementJoueur(QString nouveauJoueur);
    void activeDesactiveBtDirection();
    void majScore(QString joueurActif);
    void connexionATraiter();
    void traiteMessageRecu();
    void on_toolButtonServeur_clicked();
    void on_pushButtonConnexion_clicked();
    void envoyerModifSelection(int abscisse, int ordonne);
    void envoyerDeplacerSelection(QString direction);
};
#endif // MAINWINDOW_H
