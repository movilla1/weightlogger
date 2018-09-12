class User < ActiveRecord::Base
  # Include default devise modules. Others available are:
  # :confirmable, :lockable, :timeoutable and :omniauthable
  devise :database_authenticatable, 
         :recoverable, :rememberable, :trackable, :validatable
  # Include default devise modules. Others available are:
  # :confirmable, :lockable, :timeoutable and :omniauthable
  devise :database_authenticatable, :recoverable, :rememberable, 
         :trackable, :validatable
  extend FriendlyId
  has_many :trucks, class_name: "Camion"
  has_many :tags

  friendly_id :username, use: :slugged
  def find_with_tag(tag_id)
    tag = tags.find_by(tag_id: tag_id)
    return tag.user if tag.present?
  end
end
