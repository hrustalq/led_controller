export default {
  async getAllHosts() {
    return await import("./get-all-hosts").then((module) => module.default());
  }
}
