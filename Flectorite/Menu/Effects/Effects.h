#pragma once
#include <vector>
#include <random>
#include "../imgui_custom.h"

namespace Effects
{
    enum ParticleStyle
    {
        PARTICLE_SQUARE = 0,
        PARTICLE_CIRCLE = 1,
        PARTICLE_TRIANGLE = 2,
        PARTICLE_STAR = 3,
        PARTICLE_DIAMOND = 4
    };

    struct Particle
    {
        ImVec2 position;
        ImVec2 velocity;
        float size;
        float alpha;
        float brightness;
        float pulseOffset;
        float rotation;
        float rotationSpeed;
        float lifetime;
        float maxLifetime;
        bool active;

        Particle() : position(0, 0), velocity(0, 0), size(1.0f), alpha(1.0f), brightness(1.0f), pulseOffset(0.0f),
            rotation(0.0f), rotationSpeed(0.0f), lifetime(0.0f),
            maxLifetime(1.0f), active(false) {
        }
    };

    struct Ripple
    {
        ImVec2 center;
        float radius;
        float maxRadius;
        float alpha;
        float lifetime;
        float maxLifetime;
        bool active;
        ImU32 color;
        float timeOffset;
        int sparkleCount;
        float sparkleBrightness;
        float sparkleSize;
        float sparkleRotationSpeed;
        std::vector<float> sparkleLifetimes;
        std::vector<float> sparkleMaxLifetimes;
        std::vector<float> sparkleSizes;
        std::vector<float> sparkleTargetSizes;

        Ripple() : center(0, 0), radius(0.0f), maxRadius(100.0f), alpha(1.0f),
            lifetime(0.0f), maxLifetime(1.0f), active(false), color(0xFFFFFFFF),
            timeOffset(0.0f), sparkleCount(8), sparkleBrightness(1.0f),
            sparkleSize(2.0f), sparkleRotationSpeed(1.0f) {
        }
    };

    enum CursorStyle
    {
        CURSOR_ARROW = 0,
        CURSOR_HAND = 1,
        CURSOR_STARRY_ARROW = 2,
        CURSOR_STARRY_HAND = 3,
        CURSOR_GALAXY = 4,
        CURSOR_GALAXY_STAR = 5
    };

    struct CursorTrailPoint
    {
        ImVec2 position;
        float alpha;
        float lifetime;
        float maxLifetime;

        CursorTrailPoint() : position(0, 0), alpha(1.0f), lifetime(0.0f), maxLifetime(1.0f) {}
    };

    struct EffectColors
    {
        ImVec4 accent;
        ImVec4 glow;
        ImVec4 background;
        ImVec4 text;
        ImVec4 particleColor;
        ImVec4 rippleColor;
        ImVec4 galaxyCenter;
        ImVec4 galaxyArms;
        ImVec4 galaxySparkles;
        ImVec4 cursorColor;

        EffectColors() : accent(0.9f, 0.5f, 0.9f, 1.0f), glow(0.9f, 0.5f, 0.9f, 0.5f),
            background(0.1f, 0.1f, 0.1f, 1.0f), text(1.0f, 1.0f, 1.0f, 1.0f),
            particleColor(0.9f, 0.5f, 0.9f, 1.0f), rippleColor(0.9f, 0.5f, 0.9f, 1.0f),
            galaxyCenter(1.0f, 1.0f, 1.0f, 1.0f), galaxyArms(1.0f, 1.0f, 1.0f, 1.0f),
            galaxySparkles(1.0f, 1.0f, 1.0f, 1.0f), cursorColor(0.9f, 0.5f, 0.9f, 1.0f) {
        }
    };

    class EffectsManager
    {
    private:
        std::vector<Particle> m_particles;
        std::vector<Ripple> m_ripples;
        std::mt19937 m_generator;
        std::uniform_real_distribution<float> m_distribution;

        bool m_enableParticles;
        bool m_enableRipples;
        bool m_enableGlow;
        bool m_enableBlur;
        bool m_enableShadow;
        bool m_enableGalaxy;
        bool m_enableSparkles;
        int m_particleCount;
        float m_particleSpeed;
        float m_particleSize;
        float m_glowIntensity;
        ParticleStyle m_particleStyle;

        float m_rippleSpeed;
        float m_rippleMaxRadius;
        float m_rippleLifetime;

        float m_galaxyRotationSpeed;
        float m_galaxyArmCount;
        float m_galaxyRadius;
        float m_galaxyTime;
        int m_galaxySparkleCount;
        float m_galaxySparkleBrightness;
        float m_galaxySparkleSize;

        float m_particlePulseIntensity;
        float m_particleDriftStrength;
        float m_galaxyCenterPulseIntensity;
        float m_galaxyTwinkleSpeed;
        float m_galaxyPositionX;
        float m_galaxyPositionY;
        int m_rippleSparkleCount;
        float m_rippleSparkleBrightness;
        float m_rippleSparkleSize;
        float m_rippleSparkleRotationSpeed;

        bool m_useCustomCursor;
        CursorStyle m_cursorStyle;
        float m_cursorSize;
        float m_cursorGlowIntensity;
        bool m_enableCursorTrail;
        float m_cursorTrailLength;
        int m_cursorSparkleCount;
        float m_cursorSparkleSize;
        std::vector<CursorTrailPoint> m_cursorTrail;
        ImVec2 m_lastCursorPos;
        float m_cursorGalaxyArmCount;
        float m_cursorGalaxyRadius;
        float m_cursorGalaxyRotationSpeed;
        ImVec4 m_cursorGalaxyCenterColor;
        ImVec4 m_cursorGalaxyArmsColor;
        ImVec4 m_cursorGalaxySparklesColor;
        bool m_cursorEnableGalaxySparkles;
        int m_cursorGalaxySparkleCount;
        float m_cursorGalaxySparkleBrightness;
        float m_cursorGalaxySparkleSize;
        float m_cursorGalaxyCenterPulseIntensity;
        float m_cursorGalaxyTwinkleSpeed;
        float m_cursorGalaxyTime;

        EffectColors m_colors;

        ImVec2 m_windowPos;
        ImVec2 m_windowSize;

    public:
        EffectsManager();
        ~EffectsManager();

        void Initialize();
        void Update(float deltaTime);
        void Render(ImDrawList* drawList, const ImVec2& windowPos, const ImVec2& windowSize);
        void Shutdown();

        void InitializeParticles(const ImVec2& windowPos, const ImVec2& windowSize);
        void UpdateParticles(float deltaTime, const ImVec2& windowPos, const ImVec2& windowSize);
        void RenderParticles(ImDrawList* drawList, const ImVec2& windowPos, const ImVec2& windowSize);

        void CreateRipple(const ImVec2& position, float maxRadius = 100.0f);
        void UpdateRipples(float deltaTime);
        void RenderRipples(ImDrawList* drawList);
        void RenderRippleSparkles(ImDrawList* drawList, const Ripple& ripple);

        void RenderParticleGlow(ImDrawList* drawList, const Particle& particle, ImColor glowColor, ParticleStyle style, float glowExpansion);
        void RenderGalaxy(ImDrawList* drawList, const ImVec2& windowPos, const ImVec2& windowSize);
        void RenderCursorGalaxy(ImDrawList* drawList, const ImVec2& center, float scale);
        void RenderParticleShape(ImDrawList* drawList, const Particle& particle, ImColor color, ParticleStyle style);

        void DrawGlow(ImDrawList* drawList, const ImVec2& min, const ImVec2& max, ImU32 color, float intensity, float rounding = 0.0f);
        void DrawGlowText(ImDrawList* drawList, const ImVec2& pos, const char* text, ImColor textColor, ImColor glowColor, float intensity);

        void SetParticleCount(int count) { m_particleCount = count; }
        void SetParticleSpeed(float speed) { m_particleSpeed = speed; }
        void SetParticleSize(float size) { m_particleSize = size; }
        void SetParticleStyle(ParticleStyle style) { m_particleStyle = style; }
        void SetGlowIntensity(float intensity) { m_glowIntensity = intensity; }
        void SetColors(const EffectColors& colors) { m_colors = colors; }

        void SetRippleSpeed(float speed) { m_rippleSpeed = speed; }
        void SetRippleMaxRadius(float radius) { m_rippleMaxRadius = radius; }
        void SetRippleLifetime(float lifetime) { m_rippleLifetime = lifetime; }
        void SetRippleSparkleCount(int count) { m_rippleSparkleCount = count; }
        void SetRippleSparkleBrightness(float brightness) { m_rippleSparkleBrightness = brightness; }
        void SetRippleSparkleSize(float size) { m_rippleSparkleSize = size; }
        void SetRippleSparkleRotationSpeed(float speed) { m_rippleSparkleRotationSpeed = speed; }

        void UpdateColors(const ImVec4& accentColor, const ImVec4& glowColor)
        {
            m_colors.accent = accentColor;
            m_colors.glow = glowColor;
        }

        void SetParticleColor(const ImVec4& color) { m_colors.particleColor = color; }
        void SetRippleColor(const ImVec4& color) { m_colors.rippleColor = color; }
        void SetGalaxyCenterColor(const ImVec4& color) { m_colors.galaxyCenter = color; }
        void SetGalaxyArmsColor(const ImVec4& color) { m_colors.galaxyArms = color; }
        void SetGalaxySparklesColor(const ImVec4& color) { m_colors.galaxySparkles = color; }
        void SetCursorColor(const ImVec4& color) { m_colors.cursorColor = color; }

        void UpdateCursor(float deltaTime);
        void RenderCustomCursor(ImDrawList* drawList);
        void SetCursorStyle(CursorStyle style) { m_cursorStyle = style; }
        void SetCursorSize(float size) { m_cursorSize = size; }
        void SetCursorGlowIntensity(float intensity) { m_cursorGlowIntensity = intensity; }
        void EnableCustomCursor(bool enable) { m_useCustomCursor = enable; }
        void EnableCursorTrail(bool enable) { m_enableCursorTrail = enable; }
        void SetCursorTrailLength(float length) { m_cursorTrailLength = length; }
        void SetCursorSparkleCount(int count) { m_cursorSparkleCount = count; }
        void SetCursorSparkleSize(float size) { m_cursorSparkleSize = size; }
        void SetCursorGalaxyArmCount(float count) { m_cursorGalaxyArmCount = count; }
        void SetCursorGalaxyRadius(float radius) { m_cursorGalaxyRadius = radius; }
        void SetCursorGalaxyRotationSpeed(float speed) { m_cursorGalaxyRotationSpeed = speed; }
        void SetCursorGalaxyCenterColor(const ImVec4& color) { m_cursorGalaxyCenterColor = color; }
        void SetCursorGalaxyArmsColor(const ImVec4& color) { m_cursorGalaxyArmsColor = color; }
        void SetCursorGalaxySparklesColor(const ImVec4& color) { m_cursorGalaxySparklesColor = color; }
        void SetCursorEnableGalaxySparkles(bool enable) { m_cursorEnableGalaxySparkles = enable; }
        void SetCursorGalaxySparkleCount(int count) { m_cursorGalaxySparkleCount = count; }
        void SetCursorGalaxySparkleBrightness(float brightness) { m_cursorGalaxySparkleBrightness = brightness; }
        void SetCursorGalaxySparkleSize(float size) { m_cursorGalaxySparkleSize = size; }
        void SetCursorGalaxyCenterPulseIntensity(float intensity) { m_cursorGalaxyCenterPulseIntensity = intensity; }
        void SetCursorGalaxyTwinkleSpeed(float speed) { m_cursorGalaxyTwinkleSpeed = speed; }

        int GetParticleCount() const { return m_particleCount; }
        float GetParticleSpeed() const { return m_particleSpeed; }
        float GetParticleSize() const { return m_particleSize; }
        ParticleStyle GetParticleStyle() const { return m_particleStyle; }
        float GetGlowIntensity() const { return m_glowIntensity; }
        const EffectColors& GetColors() const { return m_colors; }

        float GetRippleSpeed() const { return m_rippleSpeed; }
        float GetRippleMaxRadius() const { return m_rippleMaxRadius; }
        float GetRippleLifetime() const { return m_rippleLifetime; }
        int GetRippleSparkleCount() const { return m_rippleSparkleCount; }
        float GetRippleSparkleBrightness() const { return m_rippleSparkleBrightness; }
        float GetRippleSparkleSize() const { return m_rippleSparkleSize; }
        float GetRippleSparkleRotationSpeed() const { return m_rippleSparkleRotationSpeed; }

        void EnableParticles(bool enable) { m_enableParticles = enable; }
        void EnableRipples(bool enable) { m_enableRipples = enable; }
        void SetEnableShadow(bool enable) { m_enableShadow = enable; }
        void SetEnableGalaxy(bool enable) { m_enableGalaxy = enable; }
        void EnableGalaxy(bool enable) { m_enableGalaxy = enable; }
        bool GetEnableGalaxy() const { return m_enableGalaxy; }

        void SetGalaxyRotationSpeed(float speed) { m_galaxyRotationSpeed = speed; }
        void SetGalaxyArmCount(float count) { m_galaxyArmCount = count; }
        void SetGalaxyRadius(float radius) { m_galaxyRadius = radius; }
        void SetGalaxySparkleCount(int count) { m_galaxySparkleCount = count; }
        void SetGalaxySparkleBrightness(float brightness) { m_galaxySparkleBrightness = brightness; }
        void SetGalaxySparkleSize(float size) { m_galaxySparkleSize = size; }
        void SetGalaxyPosition(float x, float y) { m_galaxyPositionX = x; m_galaxyPositionY = y; }
        void SetParticlePulseIntensity(float intensity) { m_particlePulseIntensity = intensity; }
        void SetParticleDriftStrength(float strength) { m_particleDriftStrength = strength; }
        void SetGalaxyCenterPulseIntensity(float intensity) { m_galaxyCenterPulseIntensity = intensity; }
        void SetGalaxyTwinkleSpeed(float speed) { m_galaxyTwinkleSpeed = speed; }

        float GetGalaxyRotationSpeed() const { return m_galaxyRotationSpeed; }
        int GetGalaxyArmCount() const { return (int)m_galaxyArmCount; }
        float GetGalaxyRadius() const { return m_galaxyRadius; }
        int GetGalaxySparkleCount() const { return m_galaxySparkleCount; }
        float GetGalaxySparkleBrightness() const { return m_galaxySparkleBrightness; }
        float GetGalaxySparkleSize() const { return m_galaxySparkleSize; }

        float GetParticlePulseIntensity() const { return m_particlePulseIntensity; }
        float GetParticleDriftStrength() const { return m_particleDriftStrength; }
        float GetGalaxyCenterPulseIntensity() const { return m_galaxyCenterPulseIntensity; }
        float GetGalaxyTwinkleSpeed() const { return m_galaxyTwinkleSpeed; }
        float GetGalaxyPositionX() const { return m_galaxyPositionX; }
        float GetGalaxyPositionY() const { return m_galaxyPositionY; }
        void EnableGlow(bool enable) { m_enableGlow = enable; }
        void EnableSparkles(bool enable) { m_enableSparkles = enable; }

        bool IsParticlesEnabled() const { return m_enableParticles; }
        bool IsRipplesEnabled() const { return m_enableRipples; }
        bool IsGlowEnabled() const { return m_enableGlow; }
        bool IsSparklesEnabled() const { return m_enableSparkles; }

        bool IsCustomCursorEnabled() const { return m_useCustomCursor; }
        CursorStyle GetCursorStyle() const { return m_cursorStyle; }
        float GetCursorSize() const { return m_cursorSize; }
        float GetCursorGlowIntensity() const { return m_cursorGlowIntensity; }
        bool IsCursorTrailEnabled() const { return m_enableCursorTrail; }
        float GetCursorTrailLength() const { return m_cursorTrailLength; }
        float GetCursorGalaxyArmCount() const { return m_cursorGalaxyArmCount; }
        float GetCursorGalaxyRadius() const { return m_cursorGalaxyRadius; }
        float GetCursorGalaxyRotationSpeed() const { return m_cursorGalaxyRotationSpeed; }
        ImVec4 GetCursorGalaxyCenterColor() const { return m_cursorGalaxyCenterColor; }
        ImVec4 GetCursorGalaxyArmsColor() const { return m_cursorGalaxyArmsColor; }
        ImVec4 GetCursorGalaxySparklesColor() const { return m_cursorGalaxySparklesColor; }
        bool GetCursorEnableGalaxySparkles() const { return m_cursorEnableGalaxySparkles; }
        int GetCursorGalaxySparkleCount() const { return m_cursorGalaxySparkleCount; }
        float GetCursorGalaxySparkleBrightness() const { return m_cursorGalaxySparkleBrightness; }
        float GetCursorGalaxySparkleSize() const { return m_cursorGalaxySparkleSize; }
        float GetCursorGalaxyCenterPulseIntensity() const { return m_cursorGalaxyCenterPulseIntensity; }
        float GetCursorGalaxyTwinkleSpeed() const { return m_cursorGalaxyTwinkleSpeed; }
    };

    extern EffectsManager g_EffectsManager;
}
