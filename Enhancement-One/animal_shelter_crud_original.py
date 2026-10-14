from pymongo import MongoClient


class AnimalShelter:
    def __init__(self, username=None, password=None):
        if username and password:
            self.client = MongoClient(
                f"mongodb://{username}:{password}@localhost:27017/aac?authSource=admin"
            )
        else:
            self.client = MongoClient("mongodb://localhost:27017")

        self.database = self.client["aac"]
        self.collection = self.database["animals"]


    def create(self, data):
        try:
            if data:
                self.collection.insert_one(data)
                return True
            return False
        except Exception as e:
            print(f"Create error: {e}")
            return False

    def read(self, query):
        try:
            results = self.collection.find(query)
            return list(results)
        except Exception as e:
            print(f"Read error: {e}")
            return []

    def update(self, query, new_values):
        try:
            result = self.collection.update_many(query, {"$set": new_values})
            return result.modified_count
        except Exception as e:
            print(f"Update error: {e}")
            return 0

    def delete(self, query):
        try:
            result = self.collection.delete_many(query)
            return result.deleted_count
        except Exception as e:
            print(f"Delete error: {e}")
            return 0
