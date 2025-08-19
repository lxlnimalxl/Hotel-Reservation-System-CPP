#include "iostream"
#include "string"
using namespace std;

class Room{
private:
    int roomId;
    string type;
    int price;
    string amenity;
    bool status;
public:
    Room(){
        cout << "room created" << endl;
        cout << "room information"<<endl;
        cout << "----------------"<<endl;
    }

    void setRoomId(int ROOMID) {
        roomId = ROOMID;
    }

    void setPrice(int PRICE) {
        price = PRICE;
    }

    void setStatus(bool STATUS) {
        status = STATUS;
    }

    void setType(string TYPE) {
        type = TYPE;
    }

    void setAmenity(string AMENITY) {
        amenity = AMENITY;
    }

    int getRoomId() const{
        return roomId;
    }

    int getPrice() const{
        return price;
    }

    string getType() const{
        return type;
    }

    string getAmenity() const{
        return amenity;
    }

    bool getStatus() const{
        return status;
    }

    void display (){
        cout << "ROOM ID : " << roomId << "\nPRICE : " << price << "\nSTATUS : " << status << "\nTYPE : " << type << "\nAMENITY : " << amenity <<endl;
    }
    ~Room(){
        cout << "----------------"<<endl;
        cout << "room deleted" << endl;
    }
};



int main (){
    Room r1;
    r1.setRoomId(22);
    r1.display();
}