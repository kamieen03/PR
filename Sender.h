#include "weapon.h"

class Sender {
	
	public:
		Sender();
		void broadcastWeaponRequest(weaponType w, float p);	//p - priorytet
		void broadcastWeaponRelease(weaponType w);
		void broadcastMedicRequest(float p);
		void broadcastMedicRelease();
		void broadcastCenterRequest(int w, float p);	//w - waga
		void broadcastCenterRelease(int w);		//w - waga
		void broadcastDeathMsg();


};
