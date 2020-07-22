#ifndef BOUTONDIRECTION_H
#define BOUTONDIRECTION_H

#include <QPushButton>

class BoutonDirection : public QPushButton
{
    Q_OBJECT
private:
    int dx;
    int dy;

public:
    BoutonDirection(QWidget *parent);
    int getDx() const;
    int getDy() const;
    void setDx(int value);
    void setDy(int value);
private slots:
    void envoyerDirection();
signals:
    void direction(QString);
};

#endif // BOUTONDIRECTION_H
