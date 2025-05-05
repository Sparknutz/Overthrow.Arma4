[EDF_DbName.Automatic()]
class PlayerBalance : EDF_DbEntity
{
    string playerId;  // Unique identifier for the player
    int balance;      // Amount of money stored in the ATM database

    void PlayerBalance(string id, int initialBalance)
    {
        playerId = id;
        balance = initialBalance;
    }
}