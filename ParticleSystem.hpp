#include "Sprite.hpp"

#include <list>
#include <memory>

class ParticleSystem :
	public sf::Drawable
{
public:
	ParticleSystem();

	// Change the source texture of the sprite instanse inside the system
	//
	// The \a texture argument refers to a texture that must
	// exist as long as the particle system uses it. Indeed, 
	// the sprite instanse inside the system
	// doesn't store its own copy of the texture, but rather keeps
	// a pointer to the one that you passed to this function.
	// If the source texture is destroyed and the system tries to
	// use it, the behavior is undefined (you will probably see 
	// something like white rects)
	//
	// parameter: texture pointer to new texture
	void setTexture(const sf::Texture* texture);

	// brief Set the global color of the sprite
	//
	// This color is modulated (multiplied) with the sprite's
	// texture. It can be used to colorize the sprite instanse 
	// inside the system, or change its global opacity.
	// By default, color is opaque white.
	//
	// parameter: new color of the sprite instanse
	// (which means recolor all the particles)
	void setColor(const sf::Color& color);

	// Set the size of the rectangle
	//
	// parameter: new size of the particles in pixels
	void setParticleSize(const sf::Vector2f& size);

	// Set the point of emission
	// 
	// Its function completely overwrites the previous point.
	// The default position of emission is (0, 0).
	// 
	// parameter: new point
	// 
	// See getEmitter
	void setEmitter(const sf::Vector2f& emitter);

	// Set the direction of emission
	// 
	// This function completely overwrites the previous direction.
	// The default direction of emission is 0.
	// 
	// parameter: new direction, as sf::Angle
	// 
	// See sf::Angle, getDirection
	void setDirection(sf::Angle direction);

	// Set the dispersion level of emission
	// 
	// This function completely overwrites the previous value.
	// The default dispersion of emission is 0.
	// 
	// parameter: new dispersion, as sf::Angle
	// 
	// Usage example:
    // code:
    // 
	// system.setDirection(sf::degrees(0.0f));
    // system.setDispersion(sf::degrees(90.0f));
	// 
	// end code.
	// 
	// Now direction of each respawned particle
	// will be in range -45 ... 45 degrees
	// 
	// See sf::Angle, getDispersion
	void setDispersion(sf::Angle dispersion);

	// Set the velocity of the particles
	// 
	// This function completely overwrites the previous direction.
	// The default velocity of particles is 0.
	// 
	// parameter: new velocity, in pixels per sec
	// 
	// See getVelocity
	void setVelocity(float velocity);

	// Set the rate of the particles respawn
	// 
	// This function completely overwrites the previous value.
	// The default rate is 0.
	// 
	// parameter: new rate
	// Explanation: setRespawnRate(4) will respawn 4 particles per second
	// 
	// See getRespawnRate
	void setRespawnRate(float rate); 

	// Set the area of the particles respawn
	// 
	// This function completely overwrites the previous value.
	// The default area is (0, 0)
	// 
	// parameter: new area in pixels
	// 
	// Particles will respawn at distanse:
	// The emission point + random2f(area)
	// Thus, it is equivalent to the following code:
	// 
	// setRespawnArea(getEmitter() + sf::Vector2f(rand() % area.x, rand() % area.y));
	// 
	// See getRespawnArea
	void setRespawnArea(const sf::Vector2f& area);

	// Set the particles lifetime
	// 
	// This function completely overwrites the previous value.
	// The default lifetime is a second as minumum ( 0 + 1.0f)
	// This value defines how long particles will continue to exist
	// 
	// parameter: new lifetime
	// 
	// See getLifeTime
	void setLifeTime(float lifetime);

	// Set the exponential scaling of the particles
	// 
	// This function completely overwrites the previous value.
	// The default value is (1.0f, 1.0f)
	// This value defines, how fast the particles will scale up
	// or scale down (if factors will be less than (1.0f, 1.0f)
	// 
	// parameter: new value
	// 
	// See getExponentialGrowth
	void setExponentialGrowth(const sf::Vector2f& factors);

	// This function enables or disables particle generation.
	// 
	// By default are disable, and always turn off when
	// explosion mode is active
	// 
	// See getEmitted
	void setEmitted(bool emitted);

	// Changing a particles opacity over time if enable
	// 
	// By default are disable
	// 
	// See getAttenuated
	void setAttenuated(bool attenuation);

	// Starts special mode: generates a certain amount
	// of particles within a user-defined radius.
	// New particles will not be generated until a 
	// old particles are exist, and then restart( if needed )
	//
	// parameters: amount of the particles, user-defined spread radius
	void setExplosion(std::size_t splash_amount, float radius);

	void update(float dt);

	const sf::Texture*  getTexture()           const;
	const sf::Color&    getColor()             const;
	const sf::Vector2f& getParticleSize()      const;
	const sf::Vector2f& getEmitter()           const;
	sf::Angle           getDirection()         const;
	sf::Angle           getDispersion()        const;
	float               getVelocity()          const;
	float               getRespawnRate()       const;
	const sf::Vector2f& getRespawnArea()       const;
	float               getLifeTime()          const;
	const sf::Vector2f& getExponentialGrowth() const;

	bool                isEmitted()    const;
	bool                isAttenuated() const;

private:
	void draw(sf::RenderTarget& target, const sf::RenderStates& states) const override;
	void createParticle();
	void setSize(sf::Sprite& sprite, const sf::Vector2f& size);
	
private:
	struct ParticleController
	{
		sf::Vector2f velocity;
		float        lifetime = 0.0f;
	};

	struct Particle
	{
		ParticleController controller;
		sf::Sprite         sprite;
	};

	std::list<std::unique_ptr<Particle>> m_particles;

	sf::Vector2f m_emitter;
	sf::Vector2f m_respawn_area;
	sf::Vector2f m_particle_size;
	sf::Vector2f m_exponential_growth;

	sf::Angle m_direction;
	sf::Angle m_dispersion;

	float m_velocity;
	float m_lifetime_max;
	float m_rate;
	float m_timer;
	

	bool m_is_emitted;
	bool m_is_attenuated;

	sf::Sprite m_instance;
};