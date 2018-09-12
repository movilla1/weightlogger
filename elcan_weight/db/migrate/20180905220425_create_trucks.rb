class CreateTrucks < ActiveRecord::Migration
  def change
    create_table :trucks do |t|
      t.string :license
      t.date :purchased
      t.float :capacity

      t.timestamps null: false
    end
  end
end
