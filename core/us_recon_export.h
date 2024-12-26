#pragma once

namespace ct {

#ifdef WIN32
  #ifdef US_RECON_CORE_SHARED_LIBS_EXPORT
    #define us_recon_core_export __declspec(dllexport)
#else
    #define us_recon_core_export __declspec(dllimport)
  #endif
#endif

}  // namespace ct