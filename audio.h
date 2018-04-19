#ifndef audio_h
#define audio_h

#include "public.h"

class AudioSystem
{
    public:
        bool muted;
        map<String, Mix_Music*> songs;
        map<String, Mix_Chunk*> sounds;

        Mix_Music* LoadSong(String path);
        Mix_Chunk* LoadSound(String path);
        AudioSystem();
        void Init();
        void Mute();
        void LoadContent();
        void PlaySong(String song);
        void PlaySound(String sound, int channel);
        void StopSound(int channel);
        void Update();
};

AudioSystem::AudioSystem(void)
{

}
void AudioSystem::Init()
{
    //Mix_Init(MIX_INIT_OGG);
    if(Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0)
        printf("Audio system failed to initialize");

    muted = g("audio-start-muted");
    if(muted)
    {
        Mix_VolumeMusic(0);
    }
    else
    {
        Mix_VolumeMusic(128);
    }
}

Mix_Music* AudioSystem::LoadSong(String path)
{
    if(path=="")
        return NULL;

    cout << "Loading " << path << "...";
    Mix_Music* mus = Mix_LoadMUS(String("data/music/"+path).c_str());
    cout << "Done!\n";
    return mus;
}
Mix_Chunk* AudioSystem::LoadSound(String path)
{
    if(path=="")
        return NULL;

    cout << "Loading " << path << "...";
    Mix_Chunk* sound = Mix_LoadWAV(String("data/music/"+path).c_str());
    cout << "Done!\n";
    return sound;
}

void AudioSystem::LoadContent()
{
    ifstream in("data/music/songs.txt");
    ifstream in2("data/music/sounds.txt");

    String line;
    while(!in.eof())
    {
        std::getline(in,line);
        //cout << FirstWord(line) << endl;
        songs[FirstWord(line)]=LoadSong(SecondWord(line));
        audioFiles++;
    };
    while(!in2.eof())
    {
        std::getline(in2,line);
        sounds[FirstWord(line)]=LoadSound(SecondWord(line));
        audioFiles++;
    }
    PlaySong("cat");

}


void AudioSystem::PlaySong(String song)
{
    if(songs.count(song))
        Mix_PlayMusic(songs[song], -1);
    //cout << "played";
}

void AudioSystem::PlaySound(String sound, int channel = -1)
{
    if(this->muted)
        return;

    cout << "playing sound" << endl;
    if(sounds.count(sound))
        Mix_PlayChannel(channel,sounds[sound], 0);
}
void AudioSystem::StopSound(int channel)
{
    Mix_HaltChannel(channel);

}
void AudioSystem::Mute()
{
    muted = !muted;
    if(muted)
    {
        Mix_VolumeMusic(0);
    }
    else
    {
        Mix_VolumeMusic(128);
    }
}
void AudioSystem::Update()
{
    while(audioCommands.size()>0)
    {
        String c = audioCommands.front();
        audioCommands.pop();
        if(c == "mute")
            Mute();
        else if(FirstWord(c)=="sound")
        {
            if(nWord(3,c)!="")
                PlaySound(SecondWord(c),atoi(nWord(3,c).c_str()));
            else
                PlaySound(SecondWord(c));
        }
        else if(FirstWord(c)=="music")
            PlaySong(SecondWord(c));
        else if(FirstWord(c)=="stop")
            StopSound(atoi(SecondWord(c).c_str()));
    }
}
#endif
