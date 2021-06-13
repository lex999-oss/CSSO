#include <windows.h>

#include <stdio.h>

#include <aclapi.h>



// Buffer clean up routine

void Cleanup(PSID pEveryoneSID, PSID pAdminSID, PACL pACL, PSECURITY_DESCRIPTOR pSD, HKEY hkSub) {

    if (pEveryoneSID)

        FreeSid(pEveryoneSID);

    if (pAdminSID)

        FreeSid(pAdminSID);

    if (pACL)

        LocalFree(pACL);

    if (pSD)

        LocalFree(pSD);

    if (hkSub)

        RegCloseKey(hkSub);
}


int main() {

    DWORD dwRes, dwDisposition;

    PSID pEveryoneSID = NULL, pAdminSID = NULL;

    PACL pACL = NULL;

    PSECURITY_DESCRIPTOR pSD = {0};

    // An array of EXPLICIT_ACCESS structure

    EXPLICIT_ACCESS explicitAccessArray[2];

    SID_IDENTIFIER_AUTHORITY SIDAuthWorld = SECURITY_WORLD_SID_AUTHORITY;

    SID_IDENTIFIER_AUTHORITY SIDAuthNT = SECURITY_NT_AUTHORITY;

    SECURITY_ATTRIBUTES securityAttributes = {0};

    LONG lRes;

    HKEY hkSub = NULL;



    // Create a well-known SID for the Everyone group.

    if (!AllocateAndInitializeSid(&SIDAuthWorld, 1, SECURITY_WORLD_RID, 0, 0, 0, 0, 0, 0, 0, &pEveryoneSID)) {

        printf("AllocateAndInitializeSid() failed, error %lu\n", GetLastError());

        Cleanup(pEveryoneSID, pAdminSID, pACL, pSD, hkSub);

    } else

        printf("SID for the Everyone group was allocated and initialized!\n");



    // Initialize an EXPLICIT_ACCESS structure for an ACE.

    // The ACE will allow Everyone read access to the key.

    ZeroMemory(&explicitAccessArray, 2 * sizeof(EXPLICIT_ACCESS));

    explicitAccessArray[0].grfAccessPermissions = KEY_READ;

    explicitAccessArray[0].grfAccessMode = SET_ACCESS;

    explicitAccessArray[0].grfInheritance = NO_INHERITANCE;

    explicitAccessArray[0].Trustee.TrusteeForm = TRUSTEE_IS_SID;

    explicitAccessArray[0].Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;

    explicitAccessArray[0].Trustee.ptstrName = (LPTSTR) pEveryoneSID;



    // Create a SID for the BUILTIN\Administrators group.

    if (!AllocateAndInitializeSid(&SIDAuthNT, 2, SECURITY_BUILTIN_DOMAIN_RID,

                                  DOMAIN_ALIAS_RID_ADMINS,

                                  0, 0, 0, 0, 0, 0,

                                  &pAdminSID)) {

        printf("AllocateAndInitializeSid() failed, error %lu\n", GetLastError());

        Cleanup(pEveryoneSID, pAdminSID, pACL, pSD, hkSub);

    } else

        printf("SID for the BUILTIN\\Administrators group was allocated and initialized!\n");



    // Initialize an EXPLICIT_ACCESS structure for an ACE.

    // The ACE will allow the Administrators group full access to the key.

    explicitAccessArray[1].grfAccessPermissions = KEY_ALL_ACCESS;

    explicitAccessArray[1].grfAccessMode = SET_ACCESS;

    explicitAccessArray[1].grfInheritance = NO_INHERITANCE;

    explicitAccessArray[1].Trustee.TrusteeForm = TRUSTEE_IS_SID;

    explicitAccessArray[1].Trustee.TrusteeType = TRUSTEE_IS_GROUP;

    explicitAccessArray[1].Trustee.ptstrName = (LPTSTR) pAdminSID;



    // Create a new ACL that contains the new ACEs.

    dwRes = SetEntriesInAcl(2, explicitAccessArray, NULL, &pACL);

    if (dwRes != ERROR_SUCCESS) {

        printf("SetEntriesInAcl() failed, error %lu\n", GetLastError());

        Cleanup(pEveryoneSID, pAdminSID, pACL, pSD, hkSub);

    } else

        printf("SetEntriesInAcl() for the Administrators group isOK\n");



    // Initialize a security descriptor.

    pSD = (PSECURITY_DESCRIPTOR) LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH);

    if (pSD == NULL) {

        printf("LocalAlloc() for pSD failed, error %lu\n", GetLastError());

        Cleanup(pEveryoneSID, pAdminSID, pACL, pSD, hkSub);

    } else

        printf("LocalAlloc() for pSD is OK !\n");


    if (!InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION)) {

        printf("InitializeSecurityDescriptor() failed, error %lu\n", GetLastError());

        Cleanup(pEveryoneSID, pAdminSID, pACL, pSD, hkSub);

    } else

        printf("InitializeSecurityDescriptor() is OK!\n");



    // Add the ACL to the security descriptor.

    if (!SetSecurityDescriptorDacl(pSD,

                                   TRUE,       // bDaclPresent flag

                                   pACL,

                                   FALSE))           // not a default DACL

    {

        printf("SetSecurityDescriptorDacl() failed, error %lu\n", GetLastError());

        Cleanup(pEveryoneSID, pAdminSID, pACL, pSD, hkSub);

    } else

        printf("SetSecurityDescriptorDacl() is OK!\n");



    // Initialize a security attributes structure.

    securityAttributes.nLength = sizeof(SECURITY_ATTRIBUTES);

    securityAttributes.lpSecurityDescriptor = pSD;

    securityAttributes.bInheritHandle = FALSE;

    // Use the security attributes to set the security descriptor when you create a registry key.

    lRes = RegCreateKeyEx(

            HKEY_CURRENT_USER,      // handle to an open key

            "Tema5",  // name of the subkey

            0,                            // Reserved

            NULL,                    // class or object type of this key, may be ignored

            0,                            // Options

            KEY_READ | KEY_WRITE, // Access right for the key

            &securityAttributes,                    // Pointer to security attribute structure, can be inherited or not. NULL is not inherited

            &hkSub,                       // variable that receives a handle to the opened or created key

            &dwDisposition);

    if (lRes == 0) {

        printf("The value of the \'dwDisposition\': %lu\n", dwDisposition);

        printf("HKEY_CURRENT_USER\\Tema5 successfully created.\n");

    } else

        printf("Creating and opening HKEY_CURRENT_USER\\Tema5 was failed, error %lu.\n", GetLastError());


    return 0;

}