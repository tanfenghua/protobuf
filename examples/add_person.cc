// See README.txt for information and build instructions.

#include <iostream>
#include <fstream>
#include <string>
#include "addressbook.pb.h"
#include <sys/time.h> 

using namespace std;

// This function fills in a Person message based on user input.
void PromptForAddress(tutorial::Person* person) {
  cout << "Enter person ID number: ";
  int id;
  cin >> id;
  person->set_id(id);
  cin.ignore(256, '\n');

  cout << "Enter name: ";
  getline(cin, *person->mutable_name());

  cout << "Enter email address (blank for none): ";
  string email;
  getline(cin, email);
  if (!email.empty()) {
    person->set_email(email);
  }

  while (true) {
    cout << "Enter a phone number (or leave blank to finish): ";
    string number;
    getline(cin, number);
    if (number.empty()) {
      break;
    }

    tutorial::Person::PhoneNumber* phone_number = person->add_phones();
    phone_number->set_number(number);

    cout << "Is this a mobile, home, or work phone? ";
    string type;
    getline(cin, type);
    if (type == "mobile") {
      phone_number->set_type(tutorial::Person::MOBILE);
    } else if (type == "home") {
      phone_number->set_type(tutorial::Person::HOME);
    } else if (type == "work") {
      phone_number->set_type(tutorial::Person::WORK);
    } else {
      cout << "Unknown phone type.  Using default." << endl;
    }
  }
}


void AutoAddPerson(tutorial::Person* person) {
  static int id = 0;
  id++;
  person->set_id(id);

  person->set_name("tanfenghua");
  person->set_email("29357216@qq.com");

  tutorial::Person::PhoneNumber* phone_number = person->add_phones();
  phone_number->set_number("189862xxxxx");
  phone_number->set_type(tutorial::Person::MOBILE);

  phone_number = person->add_phones();
  phone_number->set_number("137862xxxxx");
  phone_number->set_type(tutorial::Person::MOBILE);
}

void timeval_subtract(struct timeval* result, struct timeval* x, struct timeval* y) 
{
  int nsec; 
  if ( x->tv_sec>y->tv_sec ) 
  {
    return; 
  }

  if ( (x->tv_sec==y->tv_sec) && (x->tv_usec>y->tv_usec) ) 
  {
    return;
  }

  result->tv_sec = ( y->tv_sec-x->tv_sec ); 
  result->tv_usec = ( y->tv_usec-x->tv_usec ); 

  if (result->tv_usec<0) 
  {
    result->tv_sec--;
    result->tv_usec+=1000000;
  }
}

// Main function:  Reads the entire address book from a file,
//   adds one person based on user input, then writes it back out to the same
//   file.
int main(int argc, char* argv[]) {
  // Verify that the version of the library that we linked against is
  // compatible with the version of the headers we compiled against.
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  if (argc != 2) {
    cerr << "Usage:  " << argv[0] << " ADDRESS_BOOK_FILE" << endl;
    return -1;
  }

  tutorial::AddressBook address_book;

  {
    // Read the existing address book.
    fstream input(argv[1], ios::in | ios::binary);
    if (!input) {
      cout << argv[1] << ": File not found.  Creating a new file." << endl;
    } else if (!address_book.ParseFromIstream(&input)) {
      cerr << "Failed to parse address book." << endl;
      return -1;
    }
  }

  // Add an address.
  // PromptForAddress(address_book.add_people());
  struct timeval time_1, time_2, time_3, diff;
  
  gettimeofday(&time_1,0); 
  for (int i = 0; i < 10000; i++)
  {
    AutoAddPerson(address_book.add_people());
  }
  gettimeofday(&time_2,0); 
  timeval_subtract(&diff,&time_1,&time_2); 

  cout << "time1-2 sec:" <<diff.tv_sec << ", usec:" << diff.tv_usec << endl;

  {
    // Write the new address book back to disk.
    fstream output(argv[1], ios::out | ios::trunc | ios::binary);
    if (!address_book.SerializeToOstream(&output)) {
      cerr << "Failed to write address book." << endl;
      return -1;
    }
  }

  gettimeofday(&time_3,0); 
  memset(&diff, 0, sizeof(diff));
  timeval_subtract(&diff,&time_2,&time_3); 

  cout << "time2-3 sec:" <<diff.tv_sec << ", usec:" << diff.tv_usec << endl;

  // Optional:  Delete all global objects allocated by libprotobuf.
  google::protobuf::ShutdownProtobufLibrary();

  return 0;
}
