// #include <iostream>
// #include <stdexcept>
// #define MINIAUDIO_IMPLEMENTATION
// #include "miniaudio.h"


// #pragma region IMPORTANT
// /*
// * Voy a implementar todas las funcionalidades con ma_engine (alto nivel) y descarto ma_device (bajo nivel).
// * Porque en ma_device, no solo no existe el parser ni decodificador de los archivos, sino tambien la reproducción no es trivial.
// */
// #pragma endregion

// /*
// * @brief Initialize ma_engine
// * @param engine: reference to the ma_engine that will manage the system
// * @return MA_SUCCESS if everything has initialized correctly, or an error code otherwise
// */
// void initEngine(ma_engine& engine)
// {
//     ma_result result = ma_engine_init(NULL, &engine);
//     if (result != MA_SUCCESS)
//         throw std::runtime_error("Failed to initialize ma_engine");

//     std::cout << "ma_engine initialized successfully\n";
// }
// /*
// * @brief Release ma_engine.
// * @brief Frees all loaded sounds first, then uninitializes the engine.
// * @param engine: reference to the ma_engine that will be freed
// */
// void releaseEngine(ma_engine& engine) {
//     //release all loaded audios
//     /*for (auto a : audios) {
//         ma_sound_uninit(a);
//     } something like that*/
//     ma_engine_uninit(&engine);
// }
// /*
// * @brief Play a 2D sound once (just for testing)
// * @param engine: reference to the ma_engine that will manage the system
// * @param fileName: path to the WAV/MP3 file to load
// * @return MA_SUCCESS if everything has initialized correctly, or an error code otherwise
// */
// void playSoundEngine(ma_engine& engine, const char* fileName)
// {
//     ma_result result;
//     ma_sound sound;

//     result = ma_sound_init_from_file(&engine, 
//                                      fileName,
//                                      MA_SOUND_FLAG_NO_SPATIALIZATION, //2D sound 
//                                      NULL, //pointer to a sound group (NULL means that it does not belong to any group.)
//                                      NULL, //pointer to custom memory allocation callback (NULL means default memory management)
//                                      &sound);
//     if (result != MA_SUCCESS)
//         throw std::runtime_error(std::string("Failed to load WAV (engine): ") + fileName);

//     ma_sound_start(&sound);
//     std::cout << "Playing " << fileName << "\n";
//     //just to test if something has played
//     ma_sleep(10000);
//     ma_sound_uninit(&sound);
// }
// int foo() {
//     ma_engine engine;
//     try {
//         initEngine(engine);
//         playSoundEngine(engine, "lucky.mp3"); 
//     }
//     catch (const std::exception& e) {
//         std::cerr << "Exception: " << e.what() << std::endl;
//         releaseEngine(engine);
//         return -1;
//     }

//     releaseEngine(engine); 
//     return 0;
// }