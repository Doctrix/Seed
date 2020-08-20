#include "UEGameJoltAPI.h"
#include "Engine/Engine.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "GameJoltPluginModule.h"
#include "Misc/DateTime.h"
#include "Engine/World.h"

/* Constructor */
UUEGameJoltAPI::UUEGameJoltAPI(const class FObjectInitializer& PCIP) : Super(PCIP)
{
	Reset();
	bIsLoggedIn = false;
	GJAPI_SERVER = "api.gamejolt.com";
	GJAPI_ROOT = "/api/game/";
	GJAPI_VERSION = "v1_2";
	Game_ID = 0;
	Game_PrivateKey = "";
	LastActionPerformed = EGameJoltComponentEnum::GJ_USER_AUTH;
}

/* Prevents crashes within 'Get...' functions */
UWorld* UUEGameJoltAPI::GetWorld() const
{
	return World;
}

/* Gets the GameID */
int32 UUEGameJoltAPI::GetGameID()
{
	return Game_ID;
}

/* Sets information needed for all requests */
void UUEGameJoltAPI::Init(FString PrivateKey, int32 GameID)
{
	Game_ID = GameID;
	Game_PrivateKey = PrivateKey;
}

/* Gets the time of the GameJolt servers */
bool UUEGameJoltAPI::FetchServerTime()
{
	FString GameIDString;
	FString output;
	GameIDString = FString::FromInt(Game_ID);
	LastActionPerformed = EGameJoltComponentEnum::GJ_TIME;
	return SendRequest(output, TEXT("/time/?format=json&game_id=") + GameIDString);
}

/* Puts the requested server time in a readable format */
FDateTime UUEGameJoltAPI::ReadServerTime()
{
	UUEGameJoltAPI* responseField = NULL;
	responseField = GetObject("response");
	if (responseField == NULL)
	{
		UE_LOG(GJAPI, Error, TEXT("responseField Return Null"));
		return FDateTime();
	}
	if(!responseField->GetBool("success"))
	{
		UE_LOG(GJAPI, Error, TEXT("Can't read time: Request failed!"));
		if(responseField->GetString("message") != "")
		{
			UE_LOG(GJAPI, Error, TEXT("Error message: %s"), *responseField->GetString("message"));
		}
		return FDateTime();
	}
	int32 Year = responseField->GetInt("year");
	int32 Month = responseField->GetInt("month");
	int32 Day = responseField->GetInt("day");
	int32 Hour = responseField->GetInt("hour");
	int32 Minute = responseField->GetInt("minute");
	int32 Second = responseField->GetInt("second");

	return FDateTime(Year, Month, Day, Hour, Minute, Second);
}

/* Returns the private key */
FString UUEGameJoltAPI::GetGamePrivateKey()
{
	return Game_PrivateKey;
}

/* Returns the username */
FString UUEGameJoltAPI::GetUsername()
{
	return UserName;
}

/* Creates a new instance of the UUEGameJoltAPI class, for use in Blueprint graphs. */
UUEGameJoltAPI* UUEGameJoltAPI::Create(UObject* WorldContextObject) {
	// Get the world object from the context
	UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
	// Construct the object and return it
	UUEGameJoltAPI* fieldData = NewObject<UUEGameJoltAPI>((UUEGameJoltAPI*)GetTransientPackage(), UUEGameJoltAPI::StaticClass());
	fieldData->contextObject = WorldContextObject;
	fieldData->World = World;
	return fieldData;
}

/* Sends a request to authentificate the user */
void UUEGameJoltAPI::Login(FString name, FString token)
{
	FString output;
	FString GameIDString = FString::FromInt(Game_ID);
	LastActionPerformed = EGameJoltComponentEnum::GJ_USER_AUTH;
	UserName = name;
	UserToken = token;
	SendRequest(output, TEXT("/users/auth/?format=json&game_id=") + GameIDString + TEXT("&username=") + name + TEXT("&user_token=") + token);
}

/* Checks if the authentification was succesful */
bool UUEGameJoltAPI::isUserAuthorize()
{
	bool outAuthorize;
	UUEGameJoltAPI* responseField = NULL;
	responseField = GetObject("response");
	if (responseField == NULL)
	{
		UE_LOG(GJAPI, Error, TEXT("responseField Return Null"));
		return false;
	}
	outAuthorize = responseField->GetBool("success");
	if (!outAuthorize)
	{
		bIsLoggedIn = false;
		UE_LOG(GJAPI, Error, TEXT("Couldn't authenticate user. Message: %s"), *responseField->GetString("message"));
		return false;
	}

	bIsLoggedIn = true;
	return true;
}

/* Gets information the current user */
bool UUEGameJoltAPI::FetchUser()
{
	bool ret = true;
	FString output;
	FString GameIDString;
	GameIDString = FString::FromInt(Game_ID);
	LastActionPerformed = EGameJoltComponentEnum::GJ_USER_FETCH;
	ret = SendRequest(output, TEXT("/users/?format=json&game_id=") + GameIDString + TEXT("&username=") + UserName);
	if (!ret)
	{
		UE_LOG(GJAPI, Error, TEXT("Could not fetch user."));
		return false;
	}
	return true;
}

/* Fetches an array of users */
bool UUEGameJoltAPI::FetchUsers(TArray<int32> Users)
{
	FString output;
	LastActionPerformed = EGameJoltComponentEnum::GJ_USERS_FETCH;
	FString UserIDs = "";
	for(const int32 UserID : Users)
	{
		UserIDs.Append(FString::FromInt(UserID) + ",");
	}
	return SendRequest(output, TEXT("/users/?format=json&game_id=") + FString::FromInt(Game_ID) + "&user_id=" + UserIDs);
}

/* Fetches the friendlist of the current user */
bool UUEGameJoltAPI::FetchFriendlist()
{
	FString output;
	LastActionPerformed = EGameJoltComponentEnum::GJ_USER_FRIENDLIST;
	return SendRequest(output, "/friends/?format=json&game_id=" + FString::FromInt(Game_ID) + "&username=" + UserName + "&user_token=" + UserToken);
}

/* Gets the friendlist */
TArray<int32> UUEGameJoltAPI::GetFriendlist()
{
	TArray<UUEGameJoltAPI*> returnArray = GetObject("response")->GetObjectArray(GetObject("response"), "friends");
	TArray<int32> returnIDs;
	for(int i = 0; i < returnArray.Num(); i++)
	{
		returnIDs.Add(returnArray[i]->GetInt("friend_id"));
	}
	return returnIDs;
}

/* Resets user related properties */
void UUEGameJoltAPI::LogOffUser()
{
	bIsLoggedIn = false;
	UserName = "";
	UserToken = "";
}

/* Opens a session */
bool UUEGameJoltAPI::OpenSession()
{
	FString output;
	FString GameIDString;
	GameIDString = FString::FromInt(Game_ID);
	LastActionPerformed = EGameJoltComponentEnum::GJ_SESSION_OPEN;
	return SendRequest(output, TEXT("/sessions/open/?format=json&game_id=") + GameIDString + 
		TEXT("&username=") + UserName + TEXT("&user_token=") + UserToken);
}

/* Pings the session */
bool UUEGameJoltAPI::PingSession()
{
	FString output;
	FString GameIDString;
	GameIDString = FString::FromInt(Game_ID);
	LastActionPerformed = EGameJoltComponentEnum::GJ_SESSION_PING;
	return SendRequest(output, TEXT("/sessions/ping/?format=json&status=active&game_id=") + GameIDString +
		TEXT("&username=") + UserName + TEXT("&user_token=") + UserToken);
}

/* Closes the session */
bool UUEGameJoltAPI::CloseSession()
{
	FString output;
	FString GameIDString;
	GameIDString = FString::FromInt(Game_ID);
	LastActionPerformed = EGameJoltComponentEnum::GJ_SESSION_CLOSE;
	return SendRequest(output, TEXT("/sessions/close/?format=json&game_id=") + GameIDString +
		TEXT("&username=") + UserName +
		TEXT("&user_token=") + UserToken);
}

/* Fetches the session status */
bool UUEGameJoltAPI::CheckSession()
{
	FString output;
	LastActionPerformed = EGameJoltComponentEnum::GJ_SESSION_CHECK;
	return SendRequest(output, TEXT("/sessions/check/?format=json&game_id=") + FString::FromInt(Game_ID) + "&username=" + UserName + "&user_token=" + UserToken);
}

/* Gets the session status */
bool UUEGameJoltAPI::GetSessionStatus()
{
	UUEGameJoltAPI* Response = GetObject("response");
	if(!Response)
	{
		UE_LOG(GJAPI, Error, TEXT("Response invalid in GetSessionStatus. Was ist called to early?"));
		return false;
	}
	return Response->GetBool("success");
}

/* Deprecated - 1.8 */
TArray<FUserInfo>  UUEGameJoltAPI::FetchArrayUsers()
{
	return GetUserInfo();
}

/* Gets an array of users and puts them in an array of FUserInfo structs */
TArray<FUserInfo> UUEGameJoltAPI::GetUserInfo()
{
	TArray<UUEGameJoltAPI*> returnArray = GetObject("response")->GetObjectArray(GetObject("response"), "users");

	TArray<FUserInfo> returnUserInfo;
	
	FUserInfo tempUser;

	for (int i = 0; i< returnArray.Num(); i++)
	{

		tempUser.S_User_ID = returnArray[i]->GetInt("id");
		tempUser.User_Name = returnArray[i]->GetString("username");
		tempUser.User_Type = returnArray[i]->GetString("type");
		tempUser.User_AvatarURL = returnArray[i]->GetString("avatar_url");
		tempUser.Signed_up = returnArray[i]->GetString("signed_up");
		tempUser.Last_Logged_in = returnArray[i]->GetString("last_logged_in");
		tempUser.status = returnArray[i]->GetString("status");
		returnUserInfo.Add(tempUser);
	}
	
	return returnUserInfo;
}

/* Awards the current user a trophy */
bool UUEGameJoltAPI::RewardTrophy(int32 Trophy_ID)
{

	bool ret = true;
	FString output;
	FString GameIDString;
	FString TrophyIDString;
	GameIDString = FString::FromInt(Game_ID);
	if (!bIsLoggedIn)
	{
		UE_LOG(GJAPI, Error, TEXT("User is not logged in"));
		return false;
	}
	TrophyIDString = FString::FromInt(Trophy_ID);
	LastActionPerformed = EGameJoltComponentEnum::GJ_TROPHIES_ADD;
	ret = SendRequest(output, TEXT("/trophies/add-achieved/?format=json&game_id=") + GameIDString +
							TEXT("&username=") + UserName + 
							TEXT("&user_token=") + UserToken +
							TEXT("&trophy_id=") + TrophyIDString);


	return true;
}

/* Gets information for all trophies */
void UUEGameJoltAPI::FetchAllTrophies(EGameJoltAchievedTrophies AchievedType)
{
	TArray<int32> Trophies;
	FetchTrophies(AchievedType, Trophies);
}

/* Gets information for the selected trophies */
void UUEGameJoltAPI::FetchTrophies(EGameJoltAchievedTrophies AchievedType, TArray<int32> Trophies_ID)
{
	TArray<FTrophyInfo> returnTrophies;
	bool ret = true;
	FString output;
	FString GameIDString;
	FString TrophyIDString;
	FString AchievedString;

	if (!bIsLoggedIn)
	{
		UE_LOG(GJAPI, Error, TEXT("User is not logged in!"));
		return;
	}
	
	LastActionPerformed = EGameJoltComponentEnum::GJ_TROPHIES_FETCH;
	if(AchievedType == EGameJoltAchievedTrophies::GJ_ACHIEVEDTROPHY_GAME){

		AchievedString ="false";
	}
	else
	{
		AchievedString = "true";
	}

	GameIDString = FString::FromInt(Game_ID);
	for (int32 i = 0; i < Trophies_ID.Num(); i++){
		TrophyIDString += FString::FromInt(Trophies_ID[i]);
		if (i != Trophies_ID.Num()-1)
		{
			TrophyIDString += TEXT(",");
		}
	}
	if (AchievedType == EGameJoltAchievedTrophies::GJ_ACHIEVEDTROPHY_BLANK)//if We Want to get all trophies
	{
		ret = SendRequest(output, TEXT("/trophies/?format=json&game_id=") + GameIDString +
			TEXT("&username=") + UserName +
			TEXT("&user_token=") + UserToken +
			(Trophies_ID.Num() > 0 ? "&trophy_id=" : "") + TrophyIDString);
	}
	else //if We Want to get what trophies the User achieved have Not Achieved
	{
		ret = SendRequest(output, TEXT("/trophies/?format=json&game_id=") + GameIDString +
			TEXT("&username=") + UserName +
			TEXT("&user_token=") + UserToken +
			TEXT("&achieved=" ) + AchievedString +
			(Trophies_ID.Num() > 0 ? "&trophy_id=" : "") + TrophyIDString);
	}

	if (!ret)
	{
		UE_LOG(GJAPI, Error, TEXT("Could not fetch trophies."));
		return;
	}

	return;
}

/* Gets the trophy information from the fetched trophies */
TArray<FTrophyInfo> UUEGameJoltAPI::GetTrophies()
{
	TArray<FTrophyInfo> returnTrophy;
	TArray<UUEGameJoltAPI*> returnArray = GetObject("response")->GetObjectArray(GetObject("response"), "trophies");
	FTrophyInfo tempTrophies;
	for (int i = 0; i< returnArray.Num(); i++)
	{
		
		tempTrophies.Trophy_ID = returnArray[i]->GetInt("id");
		tempTrophies.Name = returnArray[i]->GetString("title");
		tempTrophies.Description = returnArray[i]->GetString("description");
		tempTrophies.Difficulty = returnArray[i]->GetString("difficulty");
		tempTrophies.image_url = returnArray[i]->GetString("image_url");
		tempTrophies.achieved = returnArray[i]->GetString("achieved");

		returnTrophy.Add(tempTrophies);
	}

		return returnTrophy;
}

/* Unachieves a trophy */
bool UUEGameJoltAPI::RemoveRewardedTrophy(int32 Trophy_ID)
{
	FString output;
	LastActionPerformed = EGameJoltComponentEnum::GJ_TROHIES_REMOVE;
	return SendRequest(output, TEXT("/trophies/remove-achieved/?format=json&game_id=") + FString::FromInt(Game_ID) + "&username=" + UserName + "&user_token=" + UserToken + "&trophy_id=" + FString::FromInt(Trophy_ID));
}

/* Checks if the trophy removel was successful */
bool UUEGameJoltAPI::GetTrophyRemovalStatus()
{
	UUEGameJoltAPI* Response = GetObject("response"); 
	if(!Response)
	{
		UE_LOG(GJAPI, Error, TEXT("Response invalid in GetTrophyRemovalStatus. Was ist called to early?"));
		return false;
	}
	return Response->GetBool("success");
}

/* Returns a list of scores either for a user or globally for a game */
bool UUEGameJoltAPI::FetchScoreboard(int32 ScoreLimit, int32 Table_id)
{
	TArray<FTrophyInfo> returnTrophies;
	bool ret = true;
	FString output;
	FString GameIDString;
	FString TableIDString;
	FString ScoreLimitString;

	GameIDString = FString::FromInt(Game_ID);
	TableIDString = FString::FromInt(Table_id);
	ScoreLimitString = FString::FromInt(ScoreLimit);
	LastActionPerformed = EGameJoltComponentEnum::GJ_SCORES_FETCH;

	ret = SendRequest(output, TEXT("/scores/?format=json&game_id=") + GameIDString +
		(!UserName.IsEmpty() || !bIsLoggedIn ? "&username=" : "") + UserName +
		(bIsLoggedIn ? "&user_token=" : "") + UserToken +
		(ScoreLimit > 0 ? "&limit=" : "") + (ScoreLimit > 0 ? ScoreLimitString : "") +
		(Table_id > 0 ? "&table_id=" : "") + (Table_id > 0 ? TableIDString : ""));

	if (!ret)
	{
		UE_LOG(GJAPI, Error, TEXT("Could not fetch scoreboard."));
		return false;
	}

	return true;
}

/* Gets the list of scores fetched with FetchScoreboard */
TArray<FScoreInfo> UUEGameJoltAPI::GetScoreboard()
{
	TArray<FScoreInfo> returnScoreInfo;
	TArray<UUEGameJoltAPI*> returnArray = GetObject("response")->GetObjectArray(GetObject("response"), "scores");
	FScoreInfo tempScore;
	for (int i = 0; i < returnArray.Num(); i++)
	{
		tempScore.ScoreSort = returnArray[i]->GetInt("sort");
		tempScore.ScoreString = returnArray[i]->GetString("score");
		tempScore.extra_data = returnArray[i]->GetString("extra_data");
		tempScore.UserName = returnArray[i]->GetString("user");
		tempScore.User_Id = returnArray[i]->GetInt("user_id");
		tempScore.guestUser = returnArray[i]->GetString("guest");
		tempScore.stored = returnArray[i]->GetString("stored");

		returnScoreInfo.Add(tempScore);
	}
	return returnScoreInfo;
}

/* Adds an entry to a scoreboard */
bool UUEGameJoltAPI::AddScore(FString UserScore, int32 UserScore_Sort, FString GuestUser, FString extra_data, int32 table_id)
{
	bool ret = true;
	FString output;
	FString GameIDString;
	FString TableIDString;

	GameIDString = FString::FromInt(Game_ID);
	TableIDString = FString::FromInt(table_id);
	LastActionPerformed = EGameJoltComponentEnum::GJ_SCORES_ADD;
	ret = SendRequest(output, TEXT("/scores/add/?format=json&game_id=") + GameIDString +
		TEXT("&score=") + UserScore +
		TEXT("&sort=") + FString::FromInt(UserScore_Sort) +
		(!UserName.IsEmpty() || bIsLoggedIn ? "&username=" : "") + UserName +
		(bIsLoggedIn ? "&user_token=" : "") + UserToken +
		(!bIsLoggedIn ? "&guest=" : "") + GuestUser +
		(!extra_data.IsEmpty() ? "&extra_data=" : "") + extra_data +
		(table_id > 0 ? "&table_id=" : "") + (table_id > 0 ? TableIDString : ""));
	if (!ret)
	{
		UE_LOG(GJAPI, Error, TEXT("Failed to add user's score"));
		return false;
	}

	return true;
}

/* Fetches all scoreboard tables */
bool UUEGameJoltAPI::FetchScoreboardTable()
{
	bool ret = true;
	FString output;
	FString GameIDString;

	GameIDString = FString::FromInt(Game_ID);
	LastActionPerformed = EGameJoltComponentEnum::GJ_SCORES_TABLE;

	ret = SendRequest(output, TEXT("/scores/tables/?format=json&game_id=") + GameIDString);

	if (!ret)
	{
		UE_LOG(GJAPI, Error, TEXT("Could not fetch scoreboard table"));
		return false;
	}

	return true;
}

/* Creates an array of FScoreTableInfo structs for all scoreboards of the game */
TArray<FScoreTableInfo> UUEGameJoltAPI::GetScoreboardTable()
{
	TArray<FScoreTableInfo> returnTableinfo;
	FScoreTableInfo tempTable;
	TArray<UUEGameJoltAPI*> returnArray = GetObject("response")->GetObjectArray(GetObject("response"), "tables");
	for (int i = 0; i < returnArray.Num(); i++)
	{
		tempTable.Id = returnArray[i]->GetInt("id");
		tempTable.Name = returnArray[i]->GetString("name");
		tempTable.Description = returnArray[i]->GetString("description");
		tempTable.Primary = returnArray[i]->GetString("primary");
		returnTableinfo.Add(tempTable);
	}

	return returnTableinfo;
}

/* Gets nested post data from the object with the specified key */
UUEGameJoltAPI* UUEGameJoltAPI::GetObject(const FString& key)
{
	UUEGameJoltAPI* fieldObj = NULL;
	// Try to get the object field from the data
	const TSharedPtr<FJsonObject> *outPtr;
	if (!Data->TryGetObjectField(*key, outPtr)) {
		// Throw an error and return NULL when the key could not be found
		UE_LOG(GJAPI, Error, TEXT("Entry '%s' not found in the field data!"), *key);
		return NULL;
	}

	// Create a new field data object and assign the data
	fieldObj = UUEGameJoltAPI::Create(contextObject);
	fieldObj->Data = *outPtr;

	// Return the newly created object
	return fieldObj;
}

/* Gets a string field */
FString UUEGameJoltAPI::GetString(const FString& key) const
{
	FString outString;
	if (!Data->TryGetStringField(*key, outString))
	{
		UE_LOG(GJAPI, Error, TEXT("Entry '%s' not found in the field data!"), *key);
		return "";
	}

	return outString;
}

/* Gets a bool field */
bool UUEGameJoltAPI::GetBool(const FString& key)const
{
	bool outBool;
	if (!Data->TryGetBoolField(*key,outBool))
	{
		UE_LOG(GJAPI, Error, TEXT("Entry '%s' not found in the field data!"), *key);
		return false;
	}
	return outBool;
}

/* Gets an integer field */
int32 UUEGameJoltAPI::GetInt(const FString& key) const
{
	int32 outInt;
	if (!Data->TryGetNumberField(*key, outInt))
	{
		UE_LOG(GJAPI, Error, TEXT("Entry '%s' not found in the field data!"), *key);
		return 0;
	}
	return outInt;
}

/* Gets a string array of all keys from the post data */
TArray<FString> UUEGameJoltAPI::GetObjectKeys(UObject* WorldContextObject)
{
	TArray<FString> stringArray;
	
	for (auto currJsonValue = Data->Values.CreateConstIterator(); currJsonValue; ++currJsonValue) {
		stringArray.Add((*currJsonValue).Key);
	}

	// Return the array, will be empty if unsuccessful
	return stringArray;
}

/* Gets an array of post data */
TArray<UUEGameJoltAPI*> UUEGameJoltAPI::GetObjectArray(UObject* WorldContextObject, const FString& key)
{
	TArray<UUEGameJoltAPI*> objectArray;

	// Try to fetch and assign the array to the array pointer
	const TArray<TSharedPtr<FJsonValue>> *arrayPtr;
	if (Data->TryGetArrayField(*key, arrayPtr)) {
		// Iterate through the input array and create new post data objects for every entry and add them to the objectArray
		for (int32 i = 0; i < arrayPtr->Num(); i++) {
			UUEGameJoltAPI* pageData = Create(WorldContextObject);
			pageData->Data = (*arrayPtr)[i]->AsObject();
			objectArray.Add(pageData);
		}
	}
	else {
		// Throw an error, since the value with the supplied key could not be found
		UE_LOG(GJAPI, Error, TEXT("Array entry '%s' not found in the field data!"), *key);
	}

	// Return the array, will be empty if unsuccessful

	return objectArray;
}

/* Sends a request */
bool UUEGameJoltAPI::SendRequest(const FString& output, FString url)
{

	if (Game_PrivateKey == TEXT(""))
	{
		UE_LOG(GJAPI, Error, TEXT("You must put in your game's private key before you can use any of the API functions."));
		return false;
	}

	FString outStr;
	TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<TCHAR>::Create(&outStr);
	//Start writing the response
	WriteObject(JsonWriter, "", new FJsonValueObject(Data));
	JsonWriter->Close();
	//Create URL First

	url = TEXT("https://") + GJAPI_SERVER + GJAPI_ROOT + GJAPI_VERSION + url;
	FString signature(FMD5::HashAnsiString(*(url + Game_PrivateKey))); //+ GJAPI_SERVER + url + Game_PrivateKey(TEXT("http://") + GJAPI_SERVER +
	url += TEXT("&signature=") + signature;
	UE_LOG(GJAPI, Error, TEXT("%s"), *url);


	TSharedRef< IHttpRequest > HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb("POST");
	HttpRequest->SetURL(CreateURL(url));
	HttpRequest->SetHeader("Content-Type", "application/json");
	HttpRequest->SetContentAsString(output);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UUEGameJoltAPI::OnReady);
	HttpRequest->ProcessRequest();
	
	return true;
	
}

/* Writes data */
void UUEGameJoltAPI::WriteObject(TSharedRef<TJsonWriter<TCHAR>> writer, FString key, FJsonValue* value) {
	if (value->Type == EJson::String) {
		// Write simple string entry, don't a key when it isn't set
		if (key.Len() > 0) {
			writer->WriteValue(key, value->AsString());
		}
		else {
			writer->WriteValue(value->AsString());
		}
	}
	else if (value->Type == EJson::Object) {
		// Write object entry
		if (key.Len() > 0) {
			writer->WriteObjectStart(key);
		}
		else {
			writer->WriteObjectStart();
		}

		// Loop through all the values in the object data
		TSharedPtr<FJsonObject> objectData = value->AsObject();
		for (auto objectValue = objectData->Values.CreateIterator(); objectValue; ++objectValue) {
			// Using recursion to write the key and value to the writer
			WriteObject(writer, objectValue.Key(), objectValue.Value().Get());
		}

		writer->WriteObjectEnd();
	}
	else if (value->Type == EJson::Array) {
		// Process array entry
		writer->WriteArrayStart(key);

		TArray<TSharedPtr<FJsonValue>> objectArray = value->AsArray();
		for (int32 i = 0; i < objectArray.Num(); i++) {
			// Use recursion with an empty key to process all the values in the array
			WriteObject(writer, "", objectArray[i].Get());
		}

		writer->WriteArrayEnd();
	}
}

/* Creates a http-URL from the input */
FString UUEGameJoltAPI::CreateURL(FString inputURL) {
	if (!inputURL.StartsWith("http")) {
		return "http://" + inputURL;
	}

	return inputURL;
}

/* Creates data from a string */
void UUEGameJoltAPI::FromString(const FString& dataString) {
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(dataString);

	// Deserialize the JSON data
	bool isDeserialized = FJsonSerializer::Deserialize(JsonReader, Data);

	if (!isDeserialized || !Data.IsValid()) {
		UE_LOG(GJAPI, Error, TEXT("JSON data is invalid! Input:\n'%s'"), *dataString);
	}

	// Assign the request content
	Content = dataString;
}

/* Callback for IHttpRequest::OnProcessRequestComplete() */
void UUEGameJoltAPI::OnReady(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
	if (!bWasSuccessful) {
		UE_LOG(GJAPI, Warning, TEXT("Response was invalid! Please check the URL."));

		// Broadcast the failed event
		OnFailed.Broadcast();
		return;
	}

	// Process the string
	FromString(Response->GetContentAsString());

	if(GetObject("response")->GetBool("success") == false && LastActionPerformed != EGameJoltComponentEnum::GJ_SESSION_CHECK)
	{
		OnFailed.Broadcast();
		return;
	}

	switch(LastActionPerformed)
	{
		case EGameJoltComponentEnum::GJ_USER_AUTH:
			OnUserAuthorized.Broadcast(isUserAuthorize());
			break;
		case EGameJoltComponentEnum::GJ_USER_FETCH:
			OnUserFetched.Broadcast(GetUserInfo()[0]);
			break;
		case EGameJoltComponentEnum::GJ_USERS_FETCH:
			OnUsersFetched.Broadcast(GetUserInfo());
			break;
		case EGameJoltComponentEnum::GJ_USER_FRIENDLIST:
			OnFriendlistFetched.Broadcast(GetFriendlist());
			break;
		case EGameJoltComponentEnum::GJ_SESSION_OPEN:
			OnSessionOpened.Broadcast(GetSessionStatus());
			break;
		case EGameJoltComponentEnum::GJ_SESSION_PING:
			OnSessionPinged.Broadcast(GetSessionStatus());
			break;
		case EGameJoltComponentEnum::GJ_SESSION_CLOSE:
			OnSessionClosed.Broadcast(GetSessionStatus());
			break;
		case EGameJoltComponentEnum::GJ_SESSION_CHECK:
			OnSessionChecked.Broadcast(GetSessionStatus());
			break;
		case EGameJoltComponentEnum::GJ_TROPHIES_FETCH:
			OnTrophiesFetched.Broadcast(GetTrophies());
			break;
		case EGameJoltComponentEnum::GJ_TROHIES_REMOVE:
			OnTrophyRemoved.Broadcast(GetTrophyRemovalStatus());
			break;
		case EGameJoltComponentEnum::GJ_SCORES_FETCH:
			OnScoreboardFetched.Broadcast(GetScoreboard());
			break;
		case EGameJoltComponentEnum::GJ_SCORES_TABLE:
			OnScoreboardTableFetched.Broadcast(GetScoreboardTable());
			break;
		case EGameJoltComponentEnum::GJ_TIME:
			OnTimeFetched.Broadcast(ReadServerTime());
			break;
	}
	// Broadcast the result event
	OnGetResult.Broadcast();
	return;
}

/* Resets the saved data */
void UUEGameJoltAPI::Reset()
{
	if (Data.IsValid()){
		Data.Reset();
	}

	// Created a new JSON Object
	Data = MakeShareable(new FJsonObject());
}