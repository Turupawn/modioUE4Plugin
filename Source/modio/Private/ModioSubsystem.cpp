#include "ModioSubsystem.h"
#include "ModioHWrapper.h"
#include "ModioPublic.h"
#include "ModioSettings.h"
#include "ModioModule.h"
#include "ModioUE4Utility.h"
#include "Schemas/ModioResponse.h"
#include "ModioCallbacks.h"

FModioOnModDownloadDelegate FModioSubsystem::ModioOnModDownloadDelegate;
FModioOnModUploadDelegate FModioSubsystem::ModioOnModUploadDelegate;

FModioSubsystem::FModioSubsystem() :
  bInitialized(false)
{
}

FModioSubsystem::~FModioSubsystem()
{
  check(!bInitialized);
}

FModioSubsystemPtr FModioSubsystem::Create( const FString& RootDirectory, uint32 GameId, const FString& ApiKey, bool bIsLiveEnvironent )
{
  if( !RootDirectory.Len() )
  {
    UE_LOG( LogModio, Warning, TEXT( "No root directory defined for Modio, modio will be initialized on the game directory" ) );
  }

  if( GameId == 0 )
  {
    UE_LOG( LogModio, Warning, TEXT( "No GameId defined for Modio, modio not initialized" ) );
    return nullptr;
  }

  if( !ApiKey.Len() )
  {
    UE_LOG( LogModio, Warning, TEXT( "No ApiKey defined for Modio, modio not initialized" ) );
    return nullptr;
  }

  FString GameDirectory = FPaths::ConvertRelativePathToFull( FPaths::ProjectDir() );
  GameDirectory += RootDirectory;

  FModioSubsystemPtr Modio = MakeShared<FModioSubsystem, ESPMode::Fast>();
  Modio->Init( GameDirectory, GameId, ApiKey, bIsLiveEnvironent );

  return Modio;
}

void FModioSubsystem::AddMod(const FModioModCreator& ModCreator, FModioModDelegate AddModDelegate)
{
  FModioAsyncRequest_AddMod *Request = new FModioAsyncRequest_AddMod( this, AddModDelegate );
  ModioModCreator mod_creator;
  modioInitModCreator(&mod_creator);
  SetupModioModCreator(ModCreator, mod_creator);
  modioAddMod(Request, mod_creator, FModioAsyncRequest_AddMod::Response);
  modioFreeModCreator(&mod_creator);
  QueueAsyncTask( Request );
}

void FModioSubsystem::EditMod(uint32 ModId, const FModioModEditor &ModEditor, FModioModDelegate EditModDelegate)
{
  FModioAsyncRequest_EditMod *Request = new FModioAsyncRequest_EditMod( this, EditModDelegate );
  ModioModEditor mod_editor;
  modioInitModEditor(&mod_editor);
  SetupModioModEditor(ModEditor, mod_editor);
  modioEditMod(Request , (u32)ModId, mod_editor, FModioAsyncRequest_EditMod::Response);
  modioFreeModEditor(&mod_editor);
  QueueAsyncTask( Request );
}

void FModioSubsystem::EmailExchange( const FString &SecurityCode, FModioGenericDelegate EmailExchangeDelegate )
{
  FModioAsyncRequest_EmailExchange *Request = new FModioAsyncRequest_EmailExchange( this, EmailExchangeDelegate );
  modioEmailExchange( Request, TCHAR_TO_UTF8(*SecurityCode), FModioAsyncRequest_EmailExchange::Response );
  QueueAsyncTask( Request );
}

void FModioSubsystem::EmailRequest( const FString &Email, FModioGenericDelegate EmailRequestDelegate )
{
  FModioAsyncRequest_EmailRequest *Request = new FModioAsyncRequest_EmailRequest( this, EmailRequestDelegate );
  modioEmailRequest( Request, TCHAR_TO_UTF8(*Email), FModioAsyncRequest_EmailRequest::Response );
  QueueAsyncTask( Request );
}

void FModioSubsystem::GetAllMods(TEnumAsByte<EModioFilterType> FilterType, int32 Limit, int32 Offset, FModioModArrayDelegate GetAllModsDelegate)
{
  FModioAsyncRequest_GetAllMods *Request = new FModioAsyncRequest_GetAllMods( this, GetAllModsDelegate );
  ModioFilterCreator modio_filter_creator;
  modioInitFilter(&modio_filter_creator);
  SetupModioFilterCreator(FilterType, Limit, Offset, modio_filter_creator);
  modioGetAllMods(Request, modio_filter_creator, FModioAsyncRequest_GetAllMods::Response);
  modioFreeFilter(&modio_filter_creator);
  QueueAsyncTask( Request );
}

void FModioSubsystem::GetAuthenticatedUser(FModioUserDelegate GetAuthenticatedUserDelegate)
{
  FModioAsyncRequest_GetAuthenticatedUser *Request = new FModioAsyncRequest_GetAuthenticatedUser( this, GetAuthenticatedUserDelegate );
  modioGetAuthenticatedUser(Request, FModioAsyncRequest_GetAuthenticatedUser::Response);
  QueueAsyncTask( Request );
}

void FModioSubsystem::GetUserEvents(TEnumAsByte<EModioFilterType> FilterType, int32 Limit, int32 Offset, FModioUserEventArrayDelegate GetUserEventsDelegate)
{
  FModioAsyncRequest_GetUserEvents *Request = new FModioAsyncRequest_GetUserEvents( this, GetUserEventsDelegate );
  ModioFilterCreator modio_filter_creator;
  modioInitFilter(&modio_filter_creator);
  SetupModioFilterCreator(FilterType, Limit, Offset, modio_filter_creator);
  modioGetUserEvents(Request, modio_filter_creator, FModioAsyncRequest_GetUserEvents::Response);
  modioFreeFilter(&modio_filter_creator);
  QueueAsyncTask( Request );
}

void FModioSubsystem::GetUserRatings(TEnumAsByte<EModioFilterType> FilterType, int32 Limit, int32 Offset, FModioRatingArrayDelegate GetUserRatingsDelegate)
{
  FModioAsyncRequest_GetUserRatings *Request = new FModioAsyncRequest_GetUserRatings( this, GetUserRatingsDelegate );
  ModioFilterCreator modio_filter_creator;
  modioInitFilter(&modio_filter_creator);
  SetupModioFilterCreator(FilterType, Limit, Offset, modio_filter_creator);
  modioGetUserRatings(Request, modio_filter_creator, FModioAsyncRequest_GetUserRatings::Response);
  modioFreeFilter(&modio_filter_creator);
  QueueAsyncTask( Request );
}

void FModioSubsystem::GetUserSubscriptions(TEnumAsByte<EModioFilterType> FilterType, int32 Limit, int32 Offset, FModioModArrayDelegate GetUserSubscriptionsDelegate)
{
  FModioAsyncRequest_GetUserSubscriptions *Request = new FModioAsyncRequest_GetUserSubscriptions( this, GetUserSubscriptionsDelegate );
  ModioFilterCreator modio_filter_creator;
  modioInitFilter(&modio_filter_creator);
  SetupModioFilterCreator(FilterType, Limit, Offset, modio_filter_creator);
  modioGetUserSubscriptions(Request, modio_filter_creator, FModioAsyncRequest_GetUserSubscriptions::Response);
  modioFreeFilter(&modio_filter_creator);
  QueueAsyncTask( Request );
}

void FModioSubsystem::GetUserMods(TEnumAsByte<EModioFilterType> FilterType, int32 Limit, int32 Offset, FModioModArrayDelegate GetUserModsDelegate)
{
  FModioAsyncRequest_GetUserMods *Request = new FModioAsyncRequest_GetUserMods( this, GetUserModsDelegate );
  ModioFilterCreator modio_filter_creator;
  modioInitFilter(&modio_filter_creator);
  SetupModioFilterCreator(FilterType, Limit, Offset, modio_filter_creator);
  modioGetUserMods(Request, modio_filter_creator, FModioAsyncRequest_GetUserMods::Response);
  modioFreeFilter(&modio_filter_creator);
  QueueAsyncTask( Request );
}

void FModioSubsystem::GetUserModfiles(TEnumAsByte<EModioFilterType> FilterType, int32 Limit, int32 Offset, FModioModfileArrayDelegate GetUserModfilesDelegate)
{
  FModioAsyncRequest_GetUserModfiles *Request = new FModioAsyncRequest_GetUserModfiles( this, GetUserModfilesDelegate );
  ModioFilterCreator modio_filter_creator;
  modioInitFilter(&modio_filter_creator);
  SetupModioFilterCreator(FilterType, Limit, Offset, modio_filter_creator);
  modioGetUserModfiles(Request, modio_filter_creator, FModioAsyncRequest_GetUserModfiles::Response);
  modioFreeFilter(&modio_filter_creator);
  QueueAsyncTask( Request );
}

void FModioSubsystem::SteamAuth(const FString &Base64Ticket, FModioGenericDelegate SteamAuthDelegate)
{
  FModioAsyncRequest_SteamAuth *Request = new FModioAsyncRequest_SteamAuth( this, SteamAuthDelegate );
  modioSteamAuthEncoded( Request, TCHAR_TO_UTF8(*Base64Ticket), FModioAsyncRequest_SteamAuth::Response );
  QueueAsyncTask( Request );
}

void FModioSubsystem::GalaxyAuth(const FString &Appdata, FModioGenericDelegate GalaxyAuthDelegate)
{
  FModioAsyncRequest_GalaxyAuth *Request = new FModioAsyncRequest_GalaxyAuth( this, GalaxyAuthDelegate );
  modioGalaxyAuth( Request, TCHAR_TO_UTF8(*Appdata), FModioAsyncRequest_GalaxyAuth::Response );
  QueueAsyncTask( Request );
}

void FModioSubsystem::Process()
{
  modioProcess();
}

void FModioSubsystem::Logout()
{
  modioLogout();
}
bool FModioSubsystem::IsLoggedIn()
{
  return modioIsLoggedIn();
}
FModioUser FModioSubsystem::CurrentUser()
{
  FModioUser User;
  InitializeUser(User, modioGetCurrentUser());
  return User;
}

TArray<FModioInstalledMod> FModioSubsystem::GetAllInstalledMods()
{
  TArray<FModioInstalledMod> InstalledMods;

  u32 installed_mods_count = modioGetAllInstalledModsCount();
  ModioInstalledMod *modio_installed_mods = (ModioInstalledMod *)malloc(installed_mods_count * sizeof(*modio_installed_mods));
  modioGetAllInstalledMods(modio_installed_mods);

  for (u32 i = 0; i < installed_mods_count; i++)
  {
    FModioInstalledMod installed_mod;
    InitializeInstalledMod(installed_mod, modio_installed_mods[i]);
    modioFreeInstalledMod(&modio_installed_mods[i]);
    InstalledMods.Add(installed_mod);
  }

  free(modio_installed_mods);

  return InstalledMods;
}

TArray<FModioQueuedModDownload> FModioSubsystem::GetModDownloadQueue()
{
  TArray<FModioQueuedModDownload> QueuedMods;

  u32 download_queue_count = modioGetModDownloadQueueCount();
  ModioQueuedModDownload *modio_queued_mods = (ModioQueuedModDownload *)malloc(download_queue_count * sizeof(*modio_queued_mods));
  modioGetModDownloadQueue(modio_queued_mods);

  for (u32 i = 0; i < download_queue_count; i++)
  {
    FModioQueuedModDownload queued_mod;
    InitializeQueuedModDownload(queued_mod, modio_queued_mods[i]);
    modioFreeQueuedModDownload(&modio_queued_mods[i]);
    QueuedMods.Add(queued_mod);
  }

  free(modio_queued_mods);

  return QueuedMods;
}
void FModioSubsystem::InstallDownloadedMods()
{
  modioInstallDownloadedMods();
}
void FModioSubsystem::AddModfile(int32 ModId, FModioModfileCreator ModfileCreator)
{
  ModioModfileCreator modio_modfile_creator;
  modioInitModfileCreator(&modio_modfile_creator);
  SetupModioModfileCreator(ModfileCreator, modio_modfile_creator);
  modioAddModfile((u32)ModId, modio_modfile_creator);
  modioFreeModfileCreator(&modio_modfile_creator);
}
TArray<FModioQueuedModfileUpload> FModioSubsystem::GetModfileUploadQueue()
{
  TArray<FModioQueuedModfileUpload> UploadQueue;

  u32 upload_queue_count = modioGetModfileUploadQueueCount();
  ModioQueuedModfileUpload *modio_queued_mods = (ModioQueuedModfileUpload *)malloc(upload_queue_count * sizeof(*modio_queued_mods));
  modioGetModfileUploadQueue(modio_queued_mods);

  for (u32 i = 0; i < upload_queue_count; i++)
  {
    FModioQueuedModfileUpload queued_mod;
    InitializeQueuedModfileUpload(queued_mod, modio_queued_mods[i]);
    UploadQueue.Add(queued_mod);
  }

  free(modio_queued_mods);

  return UploadQueue;
}

void FModioSubsystem::SubscribeToMod(int32 ModId, FModioModDelegate SubscribeToModDelegate)
{
  FModioAsyncRequest_SubscribeToMod *Request = new FModioAsyncRequest_SubscribeToMod( this, SubscribeToModDelegate );
  modioSubscribeToMod(Request, (u32)ModId, FModioAsyncRequest_SubscribeToMod::Response);
  QueueAsyncTask( Request );
}

void FModioSubsystem::UnsubscribeFromMod(int32 ModId, FModioGenericDelegate UnsubscribeFromModDelegate)
{
  FModioAsyncRequest_UnsubscribeFromMod *Request = new FModioAsyncRequest_UnsubscribeFromMod( this, UnsubscribeFromModDelegate );
  modioUnsubscribeFromMod(Request, (u32)ModId, FModioAsyncRequest_UnsubscribeFromMod::Response);
  QueueAsyncTask( Request );
}

void FModioSubsystem::AddModRating(int32 ModId, bool IsRatingPossitive, FModioGenericDelegate AddModRatingDelegate)
{
  FModioAsyncRequest_AddModRating *Request = new FModioAsyncRequest_AddModRating( this, AddModRatingDelegate );
  modioAddModRating(Request, (u32)ModId, IsRatingPossitive, FModioAsyncRequest_AddModRating::Response);
  QueueAsyncTask( Request );
}

void FModioSubsystem::GetAllModDependencies(int32 ModId, FModioModDependencyArrayDelegate GetAllModDependenciesDelegate)
{
  FModioAsyncRequest_GetAllModDependencies *Request = new FModioAsyncRequest_GetAllModDependencies( this, GetAllModDependenciesDelegate );
  modioGetAllModDependencies(Request, (u32)ModId, FModioAsyncRequest_GetAllModDependencies::Response);
  QueueAsyncTask( Request );
}

void FModioSubsystem::AddModDependencies(int32 ModId, const TArray<int32> &Dependencies, FModioGenericDelegate AddModDependenciesDelegate)
{
  FModioAsyncRequest_AddModDependencies *Request = new FModioAsyncRequest_AddModDependencies( this, AddModDependenciesDelegate );
  u32 *ModIds = new u32[Dependencies.Num()];
  for(int i = 0; i < Dependencies.Num(); i++)
  {
    ModIds[i] = Dependencies[i];
  }
  modioAddModDependencies(Request, (u32)ModId, ModIds, (u32)Dependencies.Num(), FModioAsyncRequest_AddModDependencies::Response);
  delete[] ModIds;

  QueueAsyncTask( Request );
}

void FModioSubsystem::DeleteModDependencies(int32 ModId, const TArray<int32> &Dependencies, FModioGenericDelegate DeleteModDependenciesDelegate)
{
  FModioAsyncRequest_DeleteModDependencies *Request = new FModioAsyncRequest_DeleteModDependencies( this, DeleteModDependenciesDelegate );
  u32 *ModIds = new u32[Dependencies.Num()];
  for(int i = 0; i < Dependencies.Num(); i++)
  {
    ModIds[i] = Dependencies[i];
  }
  modioDeleteModDependencies(Request, (u32)ModId, ModIds, (u32)Dependencies.Num(), FModioAsyncRequest_DeleteModDependencies::Response);
  delete[] ModIds;
  
  QueueAsyncTask( Request );
}

void FModioSubsystem::GetAllModTags(int32 ModId, FModioModTagArrayDelegate GetAllModTagsDelegate)
{
  FModioAsyncRequest_GetAllModTags *Request = new FModioAsyncRequest_GetAllModTags( this, GetAllModTagsDelegate );
  modioGetModTags(Request, (u32)ModId, FModioAsyncRequest_GetAllModTags::Response);
  QueueAsyncTask( Request );
}

void FModioSubsystem::AddModTags(int32 ModId, const TArray<FString> &Tags, FModioGenericDelegate AddModTagsDelegate)
{
  FModioAsyncRequest_AddModTags *Request = new FModioAsyncRequest_AddModTags( this, AddModTagsDelegate );
  char **ModTags = new char*[Tags.Num()];
  for(int i = 0; i < Tags.Num(); i++)
  {
    ModTags[i] = new char[Tags[i].Len() + 1];
    strcpy_s(ModTags[i], sizeof ModTags[i], TCHAR_TO_UTF8(*Tags[i]));
  }
  modioAddModTags(Request, (u32)ModId, ModTags, (u32)Tags.Num(), FModioAsyncRequest_AddModTags::Response);
  for(int i = 0; i < Tags.Num(); i++)
  {
    delete[] ModTags[i];
  }
  delete[] ModTags;

  QueueAsyncTask( Request );
}

void FModioSubsystem::DeleteModTags(int32 ModId, const TArray<FString> &Tags, FModioGenericDelegate DeleteModTagsDelegate)
{
  FModioAsyncRequest_DeleteModTags *Request = new FModioAsyncRequest_DeleteModTags( this, DeleteModTagsDelegate );
  char **ModTags = new char*[Tags.Num()];
  for(int i = 0; i < Tags.Num(); i++)
  {
    ModTags[i] = new char[Tags[i].Len() + 1];
    strcpy_s(ModTags[i], sizeof ModTags[i], TCHAR_TO_UTF8(*Tags[i]));
  }
  modioDeleteModTags(Request, (u32)ModId, ModTags, (u32)Tags.Num(), FModioAsyncRequest_DeleteModTags::Response);
  for(int i = 0; i < Tags.Num(); i++)
  {
    delete[] ModTags[i];
  }
  delete[] ModTags;
  
  QueueAsyncTask( Request );
}

void FModioSubsystem::GetAllMetadataKVP(int32 ModId, FModioMetadataKVPArrayDelegate GetAllMetadataKVPDelegate)
{
  FModioAsyncRequest_GetAllMetadataKVP *Request = new FModioAsyncRequest_GetAllMetadataKVP( this, GetAllMetadataKVPDelegate );
  modioGetAllMetadataKVP(Request, (u32)ModId, FModioAsyncRequest_GetAllMetadataKVP::Response);
  QueueAsyncTask( Request );
}

void FModioSubsystem::AddMetadataKVP(int32 ModId, const TMap<FString, FString> &MetadataKVP, FModioGenericDelegate AddMetadataKVPDelegate)
{
  FModioAsyncRequest_AddMetadataKVP *Request = new FModioAsyncRequest_AddMetadataKVP( this, AddMetadataKVPDelegate );
  char **CMetadataKVP = new char*[MetadataKVP.Num()];

  uint32 i = 0;
  for (const TPair<FString, FString>& pair : MetadataKVP)
  {
    FString StringfiedKVP = pair.Key + ":" + pair.Value;
    CMetadataKVP[i] = new char[StringfiedKVP.Len() + 1];
    strcpy_s(CMetadataKVP[i], sizeof StringfiedKVP, TCHAR_TO_UTF8(*StringfiedKVP));
    i++;
  }
  modioAddMetadataKVP(Request, (u32)ModId, CMetadataKVP, (u32)MetadataKVP.Num(), FModioAsyncRequest_AddMetadataKVP::Response);
  for(int i = 0; i < MetadataKVP.Num(); i++)
  {
    delete[] CMetadataKVP[i];
  }
  delete[] CMetadataKVP;

  QueueAsyncTask( Request );
}

void FModioSubsystem::DeleteMetadataKVP(int32 ModId, const TMap<FString, FString> &MetadataKVP, FModioGenericDelegate DeleteMetadataKVPDelegate)
{
  FModioAsyncRequest_DeleteMetadataKVP *Request = new FModioAsyncRequest_DeleteMetadataKVP( this, DeleteMetadataKVPDelegate );
  char **CMetadataKVP = new char*[MetadataKVP.Num()];
  
  uint32 i = 0;
  for (const TPair<FString, FString>& pair : MetadataKVP)
  {
    FString StringfiedKVP = pair.Key + ":" + pair.Value;
    CMetadataKVP[i] = new char[StringfiedKVP.Len() + 1];
    strcpy_s(CMetadataKVP[i], sizeof StringfiedKVP, TCHAR_TO_UTF8(*StringfiedKVP));
    i++;
  }
  modioDeleteMetadataKVP(Request, (u32)ModId, CMetadataKVP, (u32)MetadataKVP.Num(), FModioAsyncRequest_DeleteModTags::Response);
  for(int i = 0; i < MetadataKVP.Num(); i++)
  {
    delete[] CMetadataKVP[i];
  }
  delete[] CMetadataKVP;

  QueueAsyncTask( Request );
}

void FModioSubsystem::AddModLogo(int32 ModId, const FString &LogoPath, FModioGenericDelegate AddModLogoDelegate)
{
  FModioAsyncRequest_AddModLogo *Request = new FModioAsyncRequest_AddModLogo( this, AddModLogoDelegate );
  modioAddModLogo( Request, ModId, TCHAR_TO_UTF8(*LogoPath), FModioAsyncRequest_AddModLogo::Response );
  QueueAsyncTask( Request );
}

void FModioSubsystem::AddModImages(int32 ModId, const TArray<FString> &ImagePaths, FModioGenericDelegate AddModImagesDelegate)
{
  FModioAsyncRequest_AddModImages *Request = new FModioAsyncRequest_AddModImages( this, AddModImagesDelegate );
  char **CImagePaths = new char*[ImagePaths.Num()];
  for(int i = 0; i < ImagePaths.Num(); i++)
  {
    CImagePaths[i] = new char[ImagePaths[i].Len() + 1];
    strcpy_s(CImagePaths[i], sizeof CImagePaths[i], TCHAR_TO_UTF8(*ImagePaths[i]));
  }
  modioAddModImages(Request, (u32)ModId, CImagePaths, (u32)ImagePaths.Num(), FModioAsyncRequest_AddModImages::Response);
  for(int i = 0; i < ImagePaths.Num(); i++)
  {
    delete[] CImagePaths[i];
  }
  delete[] CImagePaths;

  QueueAsyncTask( Request );
}

void FModioSubsystem::AddModYoutubeLinks(int32 ModId, const TArray<FString> &YoutubeLinks, FModioGenericDelegate AddModYoutubeLinksDelegate)
{
  FModioAsyncRequest_AddModYoutubeLinks *Request = new FModioAsyncRequest_AddModYoutubeLinks( this, AddModYoutubeLinksDelegate );
  char **CYoutubeLinks = new char*[YoutubeLinks.Num()];
  for(int i = 0; i < YoutubeLinks.Num(); i++)
  {
    CYoutubeLinks[i] = new char[YoutubeLinks[i].Len() + 1];
    strcpy_s(CYoutubeLinks[i], sizeof CYoutubeLinks[i], TCHAR_TO_UTF8(*YoutubeLinks[i]));
  }
  modioAddModYoutubeLinks(Request, (u32)ModId, CYoutubeLinks, (u32)YoutubeLinks.Num(), FModioAsyncRequest_AddModYoutubeLinks::Response);
  for(int i = 0; i < YoutubeLinks.Num(); i++)
  {
    delete[] CYoutubeLinks[i];
  }
  delete[] CYoutubeLinks;

  QueueAsyncTask( Request );
}

void FModioSubsystem::AddModSketchfabLinks(int32 ModId, const TArray<FString> &SketchfabLinks, FModioGenericDelegate AddModSketchfabLinksDelegate)
{
  FModioAsyncRequest_AddModSketchfabLinks *Request = new FModioAsyncRequest_AddModSketchfabLinks( this, AddModSketchfabLinksDelegate );
  char **CSketchfabLinks = new char*[SketchfabLinks.Num()];
  for(int i = 0; i < SketchfabLinks.Num(); i++)
  {
    CSketchfabLinks[i] = new char[SketchfabLinks[i].Len() + 1];
    strcpy_s(CSketchfabLinks[i], sizeof CSketchfabLinks[i], TCHAR_TO_UTF8(*SketchfabLinks[i]));
  }
  modioAddModSketchfabLinks(Request, (u32)ModId, CSketchfabLinks, (u32)SketchfabLinks.Num(), FModioAsyncRequest_AddModSketchfabLinks::Response);
  for(int i = 0; i < SketchfabLinks.Num(); i++)
  {
    delete[] CSketchfabLinks[i];
  }
  delete[] CSketchfabLinks;

  QueueAsyncTask( Request );
}

void FModioSubsystem::DeleteModImages(int32 ModId, const TArray<FString> &ImagePaths, FModioGenericDelegate DeleteModImagesDelegate)
{
  FModioAsyncRequest_DeleteModImages *Request = new FModioAsyncRequest_DeleteModImages( this, DeleteModImagesDelegate );
  char **CImagePaths = new char*[ImagePaths.Num()];
  for(int i = 0; i < ImagePaths.Num(); i++)
  {
    CImagePaths[i] = new char[ImagePaths[i].Len() + 1];
    strcpy_s(CImagePaths[i], sizeof CImagePaths[i], TCHAR_TO_UTF8(*ImagePaths[i]));
  }
  modioDeleteModImages(Request, (u32)ModId, CImagePaths, (u32)ImagePaths.Num(), FModioAsyncRequest_DeleteModImages::Response);
  for(int i = 0; i < ImagePaths.Num(); i++)
  {
    delete[] CImagePaths[i];
  }
  delete[] CImagePaths;

  QueueAsyncTask( Request );
}

void FModioSubsystem::DeleteModYoutubeLinks(int32 ModId, const TArray<FString> &YoutubeLinks, FModioGenericDelegate DeleteModYoutubeLinksDelegate)
{
  FModioAsyncRequest_DeleteModYoutubeLinks *Request = new FModioAsyncRequest_DeleteModYoutubeLinks( this, DeleteModYoutubeLinksDelegate );
  char **CYoutubeLinks = new char*[YoutubeLinks.Num()];
  for(int i = 0; i < YoutubeLinks.Num(); i++)
  {
    CYoutubeLinks[i] = new char[YoutubeLinks[i].Len() + 1];
    strcpy_s(CYoutubeLinks[i], sizeof CYoutubeLinks[i], TCHAR_TO_UTF8(*YoutubeLinks[i]));
  }
  modioDeleteModYoutubeLinks(Request, (u32)ModId, CYoutubeLinks, (u32)YoutubeLinks.Num(), FModioAsyncRequest_DeleteModYoutubeLinks::Response);
  for(int i = 0; i < YoutubeLinks.Num(); i++)
  {
    delete[] CYoutubeLinks[i];
  }
  delete[] CYoutubeLinks;

  QueueAsyncTask( Request );
}

void FModioSubsystem::DeleteModSketchfabLinks(int32 ModId, const TArray<FString> &SketchfabLinks, FModioGenericDelegate DeleteModSketchfabLinksDelegate)
{
  FModioAsyncRequest_DeleteModSketchfabLinks *Request = new FModioAsyncRequest_DeleteModSketchfabLinks( this, DeleteModSketchfabLinksDelegate );
  char **CSketchfabLinks = new char*[SketchfabLinks.Num()];
  for(int i = 0; i < SketchfabLinks.Num(); i++)
  {
    CSketchfabLinks[i] = new char[SketchfabLinks[i].Len() + 1];
    strcpy_s(CSketchfabLinks[i], sizeof CSketchfabLinks[i], TCHAR_TO_UTF8(*SketchfabLinks[i]));
  }
  modioDeleteModSketchfabLinks(Request, (u32)ModId, CSketchfabLinks, (u32)SketchfabLinks.Num(), FModioAsyncRequest_DeleteModSketchfabLinks::Response);
  for(int i = 0; i < SketchfabLinks.Num(); i++)
  {
    delete[] CSketchfabLinks[i];
  }
  delete[] CSketchfabLinks;

  QueueAsyncTask( Request );
}

void FModioSubsystem::SetModDownloadListener(FModioOnModDownloadDelegate ModioOnModDownloadDelegate)
{
  FModioSubsystem::ModioOnModDownloadDelegate = ModioOnModDownloadDelegate;
}

void FModioSubsystem::SetModUploadListener(FModioOnModUploadDelegate ModioOnModUploadDelegate)
{
  FModioSubsystem::ModioOnModUploadDelegate = ModioOnModUploadDelegate;
}

void FModioSubsystem::Init( const FString& RootDirectory, uint32 GameId, const FString& ApiKey, bool bIsLiveEnvironment )
{
  check(!bInitialized);

  u32 Environment = bIsLiveEnvironment ? MODIO_ENVIRONMENT_LIVE : MODIO_ENVIRONMENT_TEST;
  
  modioInit( Environment, (u32)GameId, TCHAR_TO_UTF8(*ApiKey), TCHAR_TO_UTF8(*RootDirectory) );

  modioSetDownloadListener(&onModDownload);
  modioSetUploadListener(&onModUpload);

  bInitialized = true;
}

void FModioSubsystem::QueueAsyncTask( struct FModioAsyncRequest* Request )
{
  checkf(Request, TEXT("Trying to queue up a invalid async request"));
  TSharedPtr<FModioAsyncRequest> SharedRequest = MakeShareable(Request);
  checkf(!AsyncRequests.Contains(SharedRequest), TEXT("Trying queue a async request twice"));

  AsyncRequests.Add(SharedRequest);
}

void FModioSubsystem::AsyncRequestDone( struct FModioAsyncRequest *Request )
{
  checkf(Request, TEXT("Passing in a bad request to AsyncRequestDone"));
  checkf(AsyncRequests.Contains(Request->AsShared()), TEXT("Async Request marking itself as done multiple times"));

  AsyncRequests.RemoveSwap(Request->AsShared());
}

void FModioSubsystem::Shutdown()
{
  check(bInitialized);

  // I would assume that nullptr is valid to stop the callbacks comming in
  modioSetDownloadListener(nullptr);
  modioSetUploadListener(nullptr);

  bInitialized = false;
}


namespace ModioCallback
{
}