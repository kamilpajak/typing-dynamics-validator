#include "config.h"

#include "user.h"
#include "sample.h"
#include "keystroke.h"
#include "profile.h"

#include <iostream>
#include <mysql_driver.h>
#include <cppconn/prepared_statement.h>

int main() {
  sql::mysql::MySQL_Driver *driver = sql::mysql::get_mysql_driver_instance();
  sql::PreparedStatement *preparedStatement;
  sql::Connection *connection;
  connection = driver->connect(HOST, USER, PASSWORD);
  connection->setSchema("typing_dynamics_validator");
  preparedStatement = connection->prepareStatement("SELECT * FROM user;");
  sql::ResultSet *fetchedUsers = preparedStatement->executeQuery();
  while (fetchedUsers->next()) {
    User user;
    preparedStatement = connection->prepareStatement("SELECT * FROM sample WHERE user_id = ?");
    preparedStatement->setInt(1, fetchedUsers->getInt("id"));
    sql::ResultSet *fetchedSamples = preparedStatement->executeQuery();
    while (fetchedSamples->next()) {
      Sample sample;
      preparedStatement = connection->prepareStatement("SELECT * FROM keystroke WHERE sample_id = ?");
      preparedStatement->setInt(1, fetchedSamples->getInt("id"));
      sql::ResultSet *fetchedKeystrokes = preparedStatement->executeQuery();
      while (fetchedKeystrokes->next()) {
        Keystroke keystroke;
      }
      delete fetchedKeystrokes;
    }
    delete fetchedSamples;
  }
  delete fetchedUsers;
  delete preparedStatement;
  delete connection;

  return 0;
}
