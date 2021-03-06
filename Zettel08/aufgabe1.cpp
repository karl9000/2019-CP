#include <iostream>
#include <fstream>
#include <iomanip>
#include <Eigen/Dense>
#include <math.h>  // sqrt()

using namespace std;
using namespace Eigen;


// // Funktion des harmonischen Oszillators
VectorXd feld(VectorXd r, double m){
  return -m*r;
}

// Potential des harmonischen Oszis
double pot(VectorXd r, double m){
  return 0.5*m*r.dot(r);
}

/* Hilfsfunktion für runge_kutta
 * Berechnet aus dem Vektor y den Vektor y':
 * Schreibe die letzten d Einträge von y in die ersten d Einträge von y'
 * und die ersten d Einträge von y in die letzten d Einträge von y'
 */
VectorXd next_step(VectorXd y, double m){
  unsigned int d = y.size()/2;
  VectorXd temp(2*d);

  temp.segment(0,d) = y.segment(d,d);
  temp.segment(d,d) = 1/m*feld(y.segment(0,d),m);
  return temp;
}
/*
Funktion zur Implementierung von Runge Kutta
* f         auszuwertende Funktion
* T         obere Grenze des Zeitintervalls
* h         Schrittweite
* m         Masse
* r         Anfangswert für r
* v         Anfangswert für v
* file      File, in das geschrieben wird
* energie   Gesamtenergie des Oszillators
*/
void runge_kutta(VectorXd (*f)(VectorXd, double), double T, int N, double m,
VectorXd r, VectorXd v, ofstream &file, VectorXd &energie){
  //int N = T/h;
  double h = T/N;
  unsigned int d = r.size();
  VectorXd tn(N+1), k1, k2, k3, k4, y(2*d), y_next(2*d);
  MatrixXd ergebnis(2*d, N+1);

  // Die Startwerte in die Matrix schreiben
  ergebnis.col(0).segment(0,d) = r;
  ergebnis.col(0).segment(d,d) = v;

  // Erstellen der Zeiten tn und schreiben in ein file
  for (int i = 0; i<=N; i++){
    tn(i) = i*h;
    file << tn(i) << " ";
  }
  file << endl;

  // Initialisieren des y-Vektors
  y.segment(0,d) = r;
  y.segment(d,d) = v;

  // Berechnung der Energie aus kinetischer und potentieller Energie
  energie(0) = 0.5*m*v.dot(v) + pot(r, m);

  // Implementierung des Runge-Kutta-Verfahrens
  // Schritt 0 ist schon gemacht
  for (int i = 1; i < N+1; i++){
    k1 = h*f(y, m);
    k2 = h*f(y+0.5*k1, m);
    k3 = h*f(y+0.5*k2, m);
    k4 = h*f(y+k3, m);
    y_next = y + 1.0/6.0*(k1 + 2*k2 + 2*k3 + k4);
    y = y_next;
    ergebnis.col(i) = y;
    // Gesamtenergie berechnen
    energie(i) = 0.5*m*y.segment(d,d).dot(y.segment(d,d)) + pot(y.segment(0,d), m);
  }

  // Bestimmung der angegebenen Toleranzgrenze
  VectorXd temp;
  temp = r-y.segment(0,d);
  if(temp.norm() < 1e-5){
    cout << "Auslenkung kleiner bei h = " << h << endl;
  }

  // Schreiben der Ergebnisse in ein File
  for(int i = 0; i<ergebnis.rows()/2; i++){
    for(int j = 0; j<ergebnis.cols(); j++){
      file << ergebnis(i, j) << " ";
    }
    file << endl;
  }
}
int main() {
  cout << "Beginn des Programms!\n" << endl;
  // Initialisierung der benötigten Größen
  double T = 20.0;
  int N = 300;
  double m = 2.0;
  unsigned int d = 3;
  VectorXd r(d), v(d), energie(N+1);
  ofstream file;
  // Aufgabenteil a) mit v = 0
  r << 1, 2, 3;
  v << 0, 0, 0;

  file.open("build/a_harm.txt", ios::trunc);
  runge_kutta(next_step, T, N, m, r, v, file, energie);
  file.close();
  //cout << setprecision(13) << energie << endl;
  // Aufgabenteil b): Prüfe Energieerhaltung
  file.open("build/aufg1_c_energie.txt", ios::trunc);
  file << "zeit energie" << endl;
  for(int i=0; i<=N; i++)
  {
      file << setprecision(10) << i*T/N << " " << energie(i) << endl;
  }
  file.close();

  // Aufgabenteil a) mit r und v senkrecht
  r << 1, 0, 3;
  v << 0, 1, 0;

  file.open("build/a_unharm.txt", ios::trunc);
  runge_kutta(next_step, T, N, m, r, v, file, energie);
  file.close();

  // Aufgabenteil b)
  T = 1e-6;
  N = 10;
  runge_kutta(next_step, T, N, m, r, v, file, energie);

  cout << "\nEnde des Programms!" << endl;
  return 0;
}
