#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Dom/JsonValue.h"
#include "UEGameJoltAPI.generated.h"

/* Represents all possible requests */
UENUM(BlueprintType)
enum class EGameJoltComponentEnum : uint8
{
	GJ_USER_AUTH		UMETA(DisplayName = "Authorize User"),
	GJ_USER_FETCH		UMETA(DisplayName = "Fetch Current User"),
	GJ_USERS_FETCH		UMETA(DisplayName = "Fetch Users"),
	GJ_USER_FRIENDLIST	UMETA(DisplayName = "Fetch Friendlist"),
	GJ_SESSION_OPEN	    UMETA(DisplayName = "Open Session"),
	GJ_SESSION_PING 	UMETA(DisplayName = "Ping Session"),
	GJ_SESSION_CLOSE 	UMETA(DisplayName = "Close Session"),
	GJ_SESSION_CHECK	UMETA(DisplayName = "Check Session"),
	GJ_TROPHIES_FETCH 	UMETA(DisplayName = "Fetch Trophies"),
	GJ_TROPHIES_ADD 	UMETA(DisplayName = "Add Achieved Trophies"),
	GJ_TROHIES_REMOVE	UMETA(DisplayName = "Remove Rewarded Trophy"),
	GJ_SCORES_FETCH 	UMETA(DisplayName = "Fetch Score"),
	GJ_SCORES_ADD 		UMETA(DisplayName = "Add Score"),
	GJ_SCORES_TABLE 	UMETA(DisplayName = "Get Score Tables"),
	GJ_DATASTORE_FETCH	UMETA(DisplayName = "Fetch Data Store"),
	GJ_DATASTORE_SET	UMETA(DisplayName = "Set Data Store"),
	GJ_DATASTORE_UPDATE	UMETA(DisplayName = "Update Data Store"),
	GJ_DATASTORE_REMOVE UMETA(DisplayName = "Get Keys"),
	GJ_OTHER			UMETA(DisplayName = "Other"),
	GJ_TIME				UMETA(DisplayName = "Fetch Server Time")
};

/* Represents the possible selections for "Fetch Trophies" (all, achieved, unachieved) */
UENUM(BlueprintType)
enum class EGameJoltAchievedTrophies : uint8
{
	GJ_ACHIEVEDTROPHY_BLANK UMETA(DisplayName = "All Trophies"),
	GJ_ACHIEVEDTROPHY_USER UMETA(DisplayName = "User Achieved Trophies"),
	GJ_ACHIEVEDTROPHY_GAME UMETA(DisplayName = "Unachieved Trophies")
};

/* Contains all available information about a user */
USTRUCT(BlueprintType)
struct FUserInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "User ID")
		int32 S_User_ID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "User type")
		FString User_Type;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Username")
		FString User_Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "User Avatar")
		FString User_AvatarURL;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "User Signed up")
		FString Signed_up;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "User Last Logged in")
		FString Last_Logged_in;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "User Status")
		FString status;
};

/* Contains all information about a trophy */
USTRUCT(BlueprintType)
struct FTrophyInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trophy ID")
	int32 Trophy_ID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trophy's Name")
		FString Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trophy's Description")
		FString Description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trophy's Difficulty")
		FString Difficulty;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trophy's Image URL")
		FString image_url;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achieved Time")
		FString achieved;
};

/* Contains all information about an entry in a scoreboard */
USTRUCT(BlueprintType)
struct FScoreInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Score String")
	FString ScoreString;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Score Sort")
		int32 ScoreSort;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Score Extra Data")
		FString extra_data;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Score Username")
		FString UserName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Score User ID")
		int32 User_Id;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Score Guest Username")
		FString guestUser;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Score Stored")
		FString stored;

};

/* Contains all information about a scoreboard */
USTRUCT(BlueprintType)
struct FScoreTableInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scoreboard Table ID")
		int32 Id;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scoreboard Table Name")
		FString Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scoreboard Table Description")
		FString Description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scoreboard Table Primary")
		FString Primary;

};

/* Generates a delegate for the OnGetResult event */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGetResult);

/* Generates a delegate for the OnFailed event */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFailed);

#pragma region Specific Delegate Declaration

/* Authorize User */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUserAuthorized, bool, bIsLoggedIn);
/* Get Current User Info */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUserFetched, FUserInfo, CurrentUserInfo);
/* Get User Info*/
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUsersFetched, const TArray<FUserInfo>&, UserInfo);
/* Get Friendlist */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFriendlistFetched, const TArray<int32>&, Friendlist);
/* Open Session */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSessionOpened, bool, bIsSessionOpen);
/* Ping Session */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSessionPinged, bool, bIsSessionStillOpen);
/* Close Session */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSessionClosed, bool, bIsSessionClosed);
/* Check Session */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSessionChecked, bool, bIsSessionStillOpen);
/* Fetch Trophies */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTrophiesFetched, TArray<FTrophyInfo>, Trophies);
/* Remove Trophy */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTrophyRemoved, bool, bWasRemoved);
/* Fetch Scoreboard */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScoreboardFetched, TArray<FScoreInfo>, Scores);
/* Fetch Scoreboard Table */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScoreboardTableFetched, TArray<FScoreTableInfo>, ScoreboardTable);
/* Fetch Time */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimeFetched, struct FDateTime, ServerTime);

#pragma endregion

/**
 * Class to use the GameJoltAPI
 * Is also internally used by an UUEGameJoltAPI instance as a carrier for response data
*/
UCLASS(BlueprintType, Blueprintable)
class GAMEJOLTPLUGIN_API UUEGameJoltAPI : public UObject
{
	GENERATED_UCLASS_BODY()

private:

	/**
	* Callback for IHttpRequest::OnProcessRequestComplete()
	* @param Request HTTP request pointer
	* @param Response Response pointer
	* @param bWasSuccessful Whether the request was successful or not
	*/
	void OnReady(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	
	/* Reset Data*/
	void Reset();

	/**
	 * Creates a http-URL from the input
	 * @param inputURL The string to create the URL from
	 * @return An URL starting with http://
	*/
	static FString CreateURL(FString inputURL);

	void WriteObject(TSharedRef<TJsonWriter<TCHAR>> writer, FString key, FJsonValue* value);

public:

	UObject* contextObject;

	/* Prevents crashes in Get-Functions */
	UPROPERTY(Transient)
	class UWorld* World;

	/* Allows usage of the World-Property */
	virtual class UWorld* GetWorld() const override;

	/* The username of the guest profile */
	UPROPERTY(BlueprintReadWrite, Category = "GameJolt|User")
	FString Guest_username;

	/* Whether a user is currently logged in. Treated as a guest if false */
	UPROPERTY(BlueprintReadOnly, Category = "GameJolt|User")
	bool bIsLoggedIn;

	/* An enum representing the last request send. Local 'Get' nodes don't count */
	UPROPERTY(BlueprintReadWrite, Category = "GameJolt")
	EGameJoltComponentEnum LastActionPerformed;

	/* The actual field data */
	TSharedPtr<FJsonObject> Data;

	/* Contains the actual page content, as a string */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GameJolt|Request")
	FString Content;

	/* Event which triggers when the content has been retrieved */
	UPROPERTY(BlueprintAssignable, Category = "GameJolt|Events")
	FOnGetResult OnGetResult;

	/* Event which triggers when the request failed */
	UPROPERTY(BlueprintAssignable, Category = "GameJolt|Events")
	FOnFailed OnFailed;

#pragma region Specific Events

	UPROPERTY(BlueprintAssignable, Category = "GameJolt|Events|Specific")
	FOnUserAuthorized OnUserAuthorized;

	UPROPERTY(BlueprintAssignable, Category = "GameJolt|Events|Specific")
	FOnUserFetched OnUserFetched;

	UPROPERTY(BlueprintAssignable, Category = "GameJolt|Events|Specific")
	FOnUsersFetched OnUsersFetched;

	UPROPERTY(BlueprintAssignable, Category = "GameJolt|Events|Specific")
	FOnFriendlistFetched OnFriendlistFetched;

	UPROPERTY(BlueprintAssignable, Category = "GameJolt|Events|Specific")
	FOnSessionOpened OnSessionOpened;

	UPROPERTY(BlueprintAssignable, Category = "GameJolt|Events|Specific")
	FOnSessionPinged OnSessionPinged;

	UPROPERTY(BlueprintAssignable, Category = "GameJolt|Events|Specific")
	FOnSessionClosed OnSessionClosed;

	UPROPERTY(BlueprintAssignable, Category = "GameJolt|Events|Specific")
	FOnSessionChecked OnSessionChecked;

	UPROPERTY(BlueprintAssignable, Category = "GameJolt|Events|Specific")
	FOnTrophiesFetched OnTrophiesFetched;

	UPROPERTY(BlueprintAssignable, Category = "GameJolt|Events|Specific")
	FOnTrophyRemoved OnTrophyRemoved;

	UPROPERTY(BlueprintAssignable, Category = "GameJolt|Events|Specific")
	FOnScoreboardFetched OnScoreboardFetched;

	UPROPERTY(BlueprintAssignable, Category = "GameJolt|Events|Specific")
	FOnScoreboardTableFetched OnScoreboardTableFetched;

	UPROPERTY(BlueprintAssignable, Category = "GameJolt|Events|Specific")
	FOnTimeFetched OnTimeFetched;

#pragma endregion

	/* Creates new data from the input string */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "From String"), Category = "GameJolt|Request")
	void FromString(const FString& dataString);

	/**
	 * Creates a new instance of the UUEGameJoltAPI class, for use in Blueprint graphs.
	 * @param WorldContextObject The current context (default to self / this)
	 * @return A pointer to the newly created post data
	**/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Create GameJolt API Data", HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "GameJolt")
	static UUEGameJoltAPI* Create(UObject* WorldContextObject);

	/* GameID */
	UPROPERTY(BlueprintReadOnly, meta = (DisplayName = "Your Game ID"), Category = "GameJolt")
	int32 Game_ID;

	/* Private Key */
	UPROPERTY(BlueprintReadOnly, meta = (DisplayName = "Your Game Private Key"), Category = "GameJolt")
	FString Game_PrivateKey;

	/* Username */
	UPROPERTY(BlueprintReadOnly, meta = (DisplayName = "Players Username"), Category = "GameJolt|User")
	FString UserName;

private:
	/* Token */
	UPROPERTY()
	FString UserToken;

public:

	/* Properties for HTTP-Request*/
	UPROPERTY(BlueprintReadWrite, meta = (DisplayName = "GameJolt API Server"), Category = "GameJolt|Request")
	FString GJAPI_SERVER;

	UPROPERTY(BlueprintReadWrite, meta = (DisplayName = "GameJolt API Root"), Category = "GameJolt|Request")
	FString GJAPI_ROOT;

	UPROPERTY(BlueprintReadWrite, meta = (DisplayName = "GameJolt API Version"), Category = "GameJolt|Request")
	FString GJAPI_VERSION;
	/* End of Properties */

	/* Public Functions */

	/**
	 * Returns the game ID
	 * @return The game ID - 0 is not specified
	 **/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Your Game ID"), Category = "GameJolt")
	int32 GetGameID();

	/** Returns the private key 
	 * @return The private key - Empty if not specified
	**/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Your Game Private Key"), Category = "GameJolt")
	FString GetGamePrivateKey();
	
	/** Gets the username 
	 * @return The username - Empty if not logged in
	**/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Username"), Category = "GameJolt|User")
	FString GetUsername();

	/**
	 * Sets information needed for all requests
	 * You can find these values in the GameJolt API section of your game's dashboard
	 * @param PrivateKey The private key of your game 
	 * @param GameID The id of your game 
	 **/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Init"), Category = "GameJolt")
	void Init(FString PrivateKey, int32 GameID);

#pragma region Session

	/**
	 * Opens a session. You'll have to ping it manually with a timer
	 * @return True if the request succeded, false if not
	 **/
	UFUNCTION(BlueprintCallable, meta = (DislayName = "Open Session"), Category = "GameJolt|Sessions")
	bool OpenSession();

	/**
	 * Pings the Session. Every 30 to 60 seconds is good.
	 * @return True if the request succeded, false if not
	 **/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Ping Session"), Category = "GameJolt|Sessions")
	bool PingSession();

	/**
	 * Closes the session
	 * @return True if the request succeded, false if not
	 **/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Close Session"), Category = "GameJolt|Sessions")
	bool CloseSession();

	/**
	 * Fetches the current session status
	 * @return True if the request succeded, false if not
	 */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Fetch Session Status"), Category = "GameJolt|Sessions")
	bool CheckSession();

	/**
	 * Gets the current session status
	 * @return Whether the session is open or not. Also false if any error occurred 
	 */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Session Status"), Category = "GameJolt|Sessions")
	bool GetSessionStatus();

#pragma endregion

	/**
	 * Gets the time of the GameJolt servers
	 * @return True if the request succeded, false if not
	 **/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Fetch Server Time"), Category = "GameJolt|Misc")
	bool FetchServerTime();

	/**
	 * Puts the requested server time in a readable format
	 * UUEGameJoltAPI::FetchServerTime has to be called before this function
	 * @return The server time in a FDateTime struct
	 */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Read Server Time"), Category = "GameJolt|Misc")
	struct FDateTime ReadServerTime();

#pragma region User

	/**
	 * Sends a request to authentificate the user
	 * Call UUEGameJoltAPI::isUserAuthorize / Is User Login to check whether the authorization was successful or not
 	 * @param Name The username - case insensitive
 	 * @param Token The token - case insensitive
 	 */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Login"), Category = "GameJolt|User")
	void Login(FString Name, FString Token);

	/**
	 * Checks if the authentification was succesful
	 * @return True if the user could be logged in, false if not
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Is User Login"), Category = "GameJolt|User")
	bool isUserAuthorize();

	/* Resets user related properties */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Logoff User"), Category = "GameJolt|User")
	void LogOffUser();

	/** 
	 * Gets information about the current user
	 * @return True if it the request succeded and false if it failed
	 **/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Fetch Current User Info"), Category = "GameJolt|User")
	bool FetchUser();

	/**
	 * Fetches an array of users
	 * @param Users An array (int32) representing the user ids
	 * @return True if the request succeded, false if not
	 */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Fetch Users"), Category = "GameJolt|User")
	bool FetchUsers(TArray<int32> Users);

	/**
	 * Gets an array of users and puts them in an array of FUserInfo structs
	 * @return An array with the FUserInfo structs
	 * @deprecated Will be removed in 1.8. Use GetUserInfo() instead
	*/
	UFUNCTION(BlueprintCallable, meta = (DeprecatedFunction, DeprecationMessage = "Will be removed in 1.8. Use GetUserInfo instead", DisplayName = "Get Array of Users"), Category = "GameJolt|User")
	TArray<FUserInfo> FetchArrayUsers();

	/**
	 * Gets a single or an array of users and puts them in an array of FUserInfo structs
	 * @return An array with the FUserInfo structs
	 */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get User Info"), Category = "GameJolt|User")
	TArray<FUserInfo> GetUserInfo();

	/**
	 * Fetches the friendlist of the current user
	 * @return True if the request could be send, false if not
	 */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Fetch Friendlist"), Category = "GameJolt|User")
	bool FetchFriendlist();

	/**
	 * Returns the fetched friendlist
	 * @warning Call FetchFriendlist first
	 * @return The user ids of all friends
	 */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Friendlist"), Category = "GameJolt|User")
	TArray<int32> GetFriendlist();

#pragma endregion

#pragma region Trophies

	/** 
	 * Awards the current user a trophy
	 * @return True if the request succeded, false if not
	 **/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Reward Trophies"), Category = "GameJolt|Trophies")
	bool RewardTrophy(int32 Trophy_ID);

	/**
	 * Gets information for all trophies
	 * This is meant for the use in Blueprints
	 * It's just a wrapper around FetchTrophies with an empty TArray as an parameter
	 * You can call UUEGameJoltAPI::FetchTrophies directly
	 **/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Fetch All Trophies"), Category = "GameJolt|Trophies")
	void FetchAllTrophies(EGameJoltAchievedTrophies AchievedType);

	/**
	 * Gets information for the selected trophies
	 * @param AchievedType Whether only achieved, unachieved or all trophies should be fetched
	 * @param Tropies_ID An array of trophy IDs. An empty array will return all trophies
	 */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Fetch Trophies"), Category = "GameJolt|Trophies")
	void FetchTrophies(EGameJoltAchievedTrophies AchievedType, TArray<int32> Trophies_ID);

	/**
	 * Gets the trophy information from the fetched trophies
	 * @return Array of FTrophyInfo structs for all fetched trophies
	 */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Trophies"), Category = "GameJolt|Trophies")
	TArray<FTrophyInfo> GetTrophies();

	/**
	 * Unachieved the specified trophy for the curernt user
	 * @param Trophy_ID The ID of the trophy to be unachieved
	 * @return Whether the request could be send or not
	 */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Remove Rewarded Trophy"), Category = "GameJolt|Trophies")
	bool RemoveRewardedTrophy(int32 Trophy_ID);

	/**
	 * Checks the success of a trophy removal
	 * @return Whether the trophy was successfuly remove or not
	 */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Check Trophy Removal Status"), Category = "GameJolt|Trophies")
	bool GetTrophyRemovalStatus();

#pragma endregion

#pragma region Scores

	/**
	 * Returns a list of scores either for a user or globally for a game
	 * @return True if the request succeded, false if not
	**/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Fetch Scoreboard"), Category = "GameJolt|Scoreboard")
	bool FetchScoreboard(int32 ScoreLimit, int32 Table_id);

	/**
	 * Gets the list of scores fetched with FetchScoreboard
	 * @return An array of FScoreInfo structs for all entries
	**/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Scoreboard"), Category = "GameJolt|Scoreboard")
	TArray<FScoreInfo> GetScoreboard();

	/** 
	 * Adds an entry to a scoreboard
	 * @param UserScore A String value associated with the score. Example: "234 Jumps".
	 * @param UserScore_Sort An integer sorting value associated with the score. All sorting will work off of this number. Example: "234".
	 * @param GuestUser The guest's name. Leave blank if you're storing for a user.
	 * @param extra_data If there's any extra data you would like to store (as a string), you can use this variable. This data is never shown to the user.
	 * @param table_id The id of the high score table that you want to submit to. If left blank the score will be submitted to the primary high score table.
	 * @return True if the request succeded, false if not
	**/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Add Score to Scoreboard"), Category = "GameJolt|Scoreboard")
	bool AddScore(FString UserScore, int32 UserScore_Sort, FString GuestUser, FString extra_data, int32 table_id);

	/**
	 * Returns a list of high score tables for a game.
	 * @return True if it the request succeded and false if it failed
	 **/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Fetch Scoreboard Table"), Category = "GameJolt|Scoreboard")
	bool FetchScoreboardTable();

	/** 
	 * Gets a list of high score tables for a game and puts them in an array of FScoreTableInfo structs
	 * @return  A array of FScoreTableInfo structs
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Scoreboard Table"), Category = "GameJolt|Scoreboard")
	TArray<FScoreTableInfo>  GetScoreboardTable();

#pragma endregion

#pragma region Utility

	/* Sends Request */
	UFUNCTION(Blueprintcallable, meta = (Displayname = " Send Request"), Category = "GameJolt|Request|Advanced")
	bool SendRequest(const FString& output, FString url);

	/** Gets nested post data from the object with the specified key
	 * @param key The key of the post data value
	 * @return The value as an UUEGameJoltAPI object reference / pointer
	*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Data Field", HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "GameJolt|Request|Advanced")
	UUEGameJoltAPI* GetObject(const FString& key);

	/** Gets a string from the object with the specified key
	 * @param key The key of the string value
	 * @return The value as a string
	*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get String Field"), Category = "GameJolt|Request|Advanced")
	FString GetString(const FString& key) const;

	/** Gets a bool from the object with the specified key
	 * @param key The key of the bool value
	 * @return The value as a bool
	*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Bool Field"), Category = "GameJolt|Request|Advanced")
	bool GetBool(const FString& key) const;

	/** Gets an integer from the object with the specified key
	 * @param key The key of the integer value
	 * @return The value as an integer
	*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Int Field"), Category = "GameJolt|Request|Advanced")
	int32 GetInt(const FString& key) const;

	/**
	 * Gets a string array of all keys from the post data
	* @return An array with all keys
	*/
	UFUNCTION(Blueprintpure, meta = (Displayname = "Get Object Keys", HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "GameJolt|Request|Advanced")
	TArray<FString> GetObjectKeys(UObject* WorldContextObject);

	/**
	 * Gets an array fromt the post data
	 * @param key The key of the array
	 * @return The array assigned to the key
	 **/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Object Array Field", HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "GameJolt|Request|Advanced")
	TArray<UUEGameJoltAPI*> GetObjectArray(UObject* WorldContextObject, const FString& key);

#pragma endregion

};