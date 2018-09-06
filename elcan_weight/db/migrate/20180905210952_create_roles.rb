class CreateRoles < ActiveRecord::Migration
  def change
    create_table :roles do |t|
      t.references :usuario, index: true, foreign_key: true
      t.integer :role

      t.timestamps null: false
    end
  end
end
