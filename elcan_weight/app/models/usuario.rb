class Usuario < ActiveRecord::Base
  # Include default devise modules. Others available are:
  # :confirmable, :lockable, :timeoutable and :omniauthable
  devise :database_authenticatable, :recoverable, :rememberable, 
         :trackable, :validatable
  extend FriendlyId
  friendly_id :username, use: :slugged
end
