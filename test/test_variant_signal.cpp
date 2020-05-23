#include <gmock/gmock.h>
#include <wkttool/variant_signal.h>

using ::testing::MockFunction;
using ::testing::StrictMock;
  
TEST(TestVariantSignal, CorrectListenerIsNotified) {
  using Variant = std::variant<std::string, int>;
  wkttool::VariantSignal<Variant> sig;
  StrictMock<MockFunction<void(const std::string&)>> mock_string_listener;
  StrictMock<MockFunction<void(const int&)>> mock_int_listener;
  sig.connect(mock_string_listener.AsStdFunction());
  sig.connect(mock_int_listener.AsStdFunction());

  EXPECT_CALL(mock_string_listener, Call("Hello"));
  EXPECT_CALL(mock_int_listener, Call(42));

  sig(std::string("Hello"));
  sig(42);
}
TEST(TestVariantSignal, CorrectListenerIsNotifiedImplicitConversion) {
  using Variant = std::variant<std::string, int>;
  wkttool::VariantSignal<Variant> sig;
  StrictMock<MockFunction<void(const std::string&)>> mock_string_listener;
  sig.connect(mock_string_listener.AsStdFunction());

  EXPECT_CALL(mock_string_listener, Call("Hello"));

  sig("Hello");
}
TEST(TestVariantSignal, LambdaListenerIsNotified) {
  using Variant = std::variant<std::string, int>;
  wkttool::VariantSignal<Variant> sig;
  bool called = false;
  sig.connect([&called] (const std::string&) {
    called = true;});

  sig(std::string("Hello"));
  EXPECT_TRUE(called);
}
TEST(TestVariantSignal, FunctorListenerIsNotified) {
  using Variant = std::variant<std::string>;
  wkttool::VariantSignal<Variant> sig;
  bool called = false;
  struct Functor {
    bool& called = called;
    void operator()(const std::string&) {
      called = true;
    }
  };
  Functor f;
  sig.connect(f);

  sig(std::string("Hello"));
  EXPECT_TRUE(called);
}
TEST(TestVariantSignal, NoNotificationAfterDisconnect) {
  using Variant = std::variant<std::string>;
  wkttool::VariantSignal<Variant> sig;
  StrictMock<MockFunction<void(const std::string&)>> mock_string_listener;
  auto connection = sig.connect(mock_string_listener.AsStdFunction());
  connection.disconnect();
  // If this triggers a notification, the strict mock will complain
  sig(std::string("Hello"));

}
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
